import discord
from discord.ext import commands
import wavelink

client = commands.Bot(command_prefix="!", intents=discord.Intents.all())

class CustomPlayer(wavelink.Player):

    def __init__(self):
        super().__init__()
        self.queue = wavelink.Queue()


@client.event
async def on_ready():
    client.loop.create_task(connect_nodes())

async def connect_nodes():
    await client.wait_until_ready()
    await wavelink.NodePool.create_node(
        bot=client,
        host='127.0.0.1',
        port=2333,
        password='youshallnotpass'
    )

@client.event
async def on_wavelink_node_ready(node: wavelink.Node):
    print(f'Node: <{node.identifier}> is ready!')

@client.event
async def on_wavelink_track_end(player: CustomPlayer, track: wavelink.Track, reason):
    if not player.queue.is_empty:
        next_track = player.queue.get()
        await player.play(next_track)

@client.command()
async def connect(ctx):
    vc = ctx.voice_client
    try:
        channel = ctx.author.voice.channel
    except AttributeError:
        return await ctx.send("Please join a voice channel to connect")

    if not vc:
        await ctx.author.voice.channel.connect(cls=CustomPlayer())
    else:
        await ctx.send("The bot is already connected to a voice channel")

@client.command()
async def disconnect(ctx):
    vc = ctx.voice_client
    if vc:
        await vc.disconnect()
    else:
        await ctx.send("The bot is not connected to a voice channel")

@client.command()
async def play(ctx, *, search: wavelink.YouTubeTrack):
    vc = ctx.voice_client
    if not vc:
        custom_player = CustomPlayer()
        vc: CustomPlayer = await ctx.author.voice.channel.connect(cls=custom_player)

    if vc.is_playing():

        vc.queue.put(item=search)

        await ctx.send(embed=discord.Embed(
            title=search.title,
            url=search.uri,
            author=ctx.author,
            description=f"Queued {search.title} in {vc.channel}"
        ))
    else:
        await vc.play(search)

        await ctx.send(embed=discord.Embed(
            title=vc.source.title,
            url=vc.source.uri,
            author=ctx.author,
            description=f"Playing {vc.source.title} in {vc.channel}"
        ))

@client.command()
async def skip(ctx):
    vc = ctx.voice_client
    if vc:
        if not vc.is_playing():
            return await ctx.send("Nothing is playing.")
        if vc.queue.is_empty:
            return await vc.stop()

        await vc.seek(vc.track.length * 1000)
        if vc.is_paused():
            await vc.resume()
    else:
        await ctx.send("The bot is not connected to a voice channel.")

@client.command()
async def pause(ctx):
    vc = ctx.voice_client
    if vc:
        if vc.is_playing() and not vc.is_paused():
            await vc.pause()
        else:
            await ctx.send("Nothing is playing")
    else:
        await ctx.send("The bot is not connected to a voice channel")

@client.command()
async def resume(ctx):
    vc = ctx.voice_client
    if vc:
        if vc.is_paused():
            await vc.resume()
        else:
            await ctx.send("It is already playing")

    else:
        await ctx.send("The bot is not connected to a voice channel")


@play.error
async def play_error(ctx, error):
    if isinstance(error, commands.BadArgument):
        await ctx.send("could not find a track.")
    else:
        await ctx.send("Please join a voice channel")

client.run('MTA2NDI4OTUyMjkxNDk2MzUzNg.GRM6cM.QeYOwiXq64zMtrfmAHxt_17DCSrT6SAxHzgfPQ')

