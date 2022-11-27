# AMABot

## Description

This bot is designed to take user input from a discord user, run it through a neural network that does text generation based on that input, and reply back with the response. Depending on the slash command, the bot will send the user input to a specific AI model located on some HTTP server. The first parameter is a string of what the user wants to say to the AI. The models can be hosted on the same or different servers, and are  interfaced with through REST calls by some url specified in a JSON configuration file.

## Dependencies

### Build requirements
* Only builds for unix-based systems. Windows not currently supported.
* [cmake](https://cmake.org/) (version 3.14+)
* A supported C++ compiler like g++ (https://gcc.gnu.org)

### External Dependencies (Must be installed before trying to build)

#### DPP requires:

* [OpenSSL](https://openssl.org/) (whichever `-dev` package comes with your OS)
* [zlib](https://zlib.net) (whichever `-dev` package comes with your OS)

#### Curlpp requires:

* [libcurl](https://curl.se/) (whichever `-dev-openssl`)

## Building

Once all the dependencies have been acquired. From root directory:

```
cmake -B ./build
make -C ./build
```

## JSON Config

The bot requires a json configuration file to know various aspects of
itself during runtime, and where to route REST calls for the AI models.
By default, a *config.json* file is expected to be at the root directory.
If wanting to use a different json file, it can also be passed as the first
parameter to the program. Below is the minimum Json required for the bot,
followed by a brief description of any properties that may need more explanation.

```json
{
    "token": "YOUR BOT TOKEN HERE",
    "num_pool_threads": 8,
    "models":
    {
        "MODEL NAME": 
        {
            "name": "Test Model",
            "url": "URL TO SERVER",
            "endpoints":
            {
                "ping": "/",
                "send_input":
                {
                    "path": "/model/test",
                    "req_body": "user_input",
                    "res_body": "reply"
                }
            }
        }
    }
}
```

- **num_pool_threads** - The number of thread pool threads to handle REST requests. By default this is set to `std::thread::hardware_concurrency()`. This is an optional attribute.
- **models** - An object containing all the AI models the bot can use. Each key represents a single model. Each value is an object containing more information needed for the bot to interface with the model. Should be named exactly this.
- **MODEL NAME** - This is a name refered to internally by the C++ code, and how the bot will refer to this specific model. This can be any name. This is what gets passed into the slash command callbacks using the parameter "lModel". Every model must contain at least what is described in the JSON above. There can be any number of these.
- **name** - This is what gets printed out to the user whenever it needs to refer to the model.
- **url** - Base url of the server the model is hosted on.
- **endpoints** - An object containing any endpoints needed for the discord commands to interface with the model. Each key represents a separate endpoint.
- **ping** - Path used for checking if server is still online.
- **send_input** - Endpoint for sending user text to the model.
- **path** - Path to send the user text to for the model to process and generate a response.
- **req_body** - The JSON property the server expects the input to be in.
- **res_body** - The JSON property the server will put the output in.

## Running the bot

```
cd build
./AMAbot
```
OR
```
/path/to/bot/executable/AMAbot /path/to/json_config/SOME_NAME.json
```

## Known limitations

- Each thread in the thread pool runs a blocking libcurl post request to the AI model's url. Since the model can take a none trivial amount of time to respond back with a reply, this can affect the user experience. Especially if a large number of guilds are hammering away with many requests at once. There will certainly be a noticeable impact. As the goals of this project was meant only to serve one or two servers, this is still overkill. In normal circumstances, this limitation likely won't be an issue as the bot/model should still adequately service a reasonable amount of requests; even for a small number of guilds.

    - If I wanted to reduce this limitation, I would use libcurl's multi interface instead of it's easy interface. The multi interface would run in a single thread and each user command request would be added to a queue. The multi handle could poll this queue and issue requests at once in a nonblocking manner. On receipt of a response from a post call, it would get added to the thread pool's queue for further processing. This should reduce the impact of the limitiation. The reason I did not do the above is twofold. One, the current implementation is still more than enough to fit the needs of the project. And two, libcurl's documenation is not friendly at all and it didn't sound like fun digging through and trying to get a working implementation. At first, second, and third glances, it was not intuitive to do anything other than come up with this high level design. I may come back to it if the need arises but for now, that exercise is left to the reader ;)
    - Another possibility I did try but ended up ditching was using DPP's built-in REST calls for the AI models. I ran into two roadblocks with their implementation, however. One, it seems like the calls are executed in a synchronous fashion if the url is the same. Each http request runs in exactly the same thread if the request has the same url. That sounds like a worse alternative than the current implementation. The second roadblock was that DPP, for some reason, sets a timeout for requests to 5 seconds. With no way to change it at the API level. This timeout is way too little for the needs of this project. If I wanted to change this, digging through the code and "hacking in" this settable timeout looked messy. Especially considering the first roadblock, I opted not to go that route.
    - Another option is to just ditch the REST calls altogether and use web sockets with a fire-and-forget messaging scheme understood by the bot and the servers. This would reduce potential bottlenecks of the bot. A socket can be opened for each server the models live on. They can all be in one thread (or multiple if communicating with multiple servers). When a socket receives a message, it can dump it to a separate thread for routing/processing.
    - Of course, one quick and dirty way to reduce the limitation is to simply increase the number of threads in the thread pool. Obviously this just kicks the problem down the road, but it's still a pretty strong kick...
