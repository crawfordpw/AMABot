# AMABot

#### TODO : Add description

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

* [libcurl](https://curl.se/) (whichever `-dev` package. Using openssl currently)

## Building

Once all the dependencies have been acquired:

```
cmake -B ./build
make -C ./build
```

## JSON Config

The bot requires a json configuration file to know various aspects of
itself during runtime, and where to route REST calls for the AI models.
By default, a *config.json* file is expected to be at the root directory.
If wanting to use a different json file it can also be passed as the first
to the program. Below is the minimum Json required for the bot, followed by
a brief description of any properties that may need more explanation.

```json
{
    "token": "YOUR BOT TOKEN HERE",
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
