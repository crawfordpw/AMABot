#####################################################################
##
## Server.py
##
## Main file for running the REST server.
##
#####################################################################

import uvicorn
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import Models

"""
TODO

Add api call:
    - GET current model running
        - This will allow discord to query if model is running
        before trying to send it input.
"""

## Our FastAPI application.
gApp = FastAPI()

########
# Structure for input coming from Discord.
########
class Body(BaseModel):
    user_input: str

@gApp.on_event('startup')
def init_data():
    Models.InitModels()
    return

########
# Landing page? Should this be the ping endpoint instead?
########
@gApp.get("/")
async def root():
    return {"reply": "Hello World"}

########
# Used for determining if the server is up or not.
########
@gApp.get("/ping")
def Ping():
    return {"reply": "pong"}

########
# TODO Fill this in with more robust check.
########
@gApp.get("/current_model")
def GetCurrentModel():
    return {"reply": ""};

########
# Main call for the server. This will send off the user
# input from Discord to the model for generating a reply back.
# TODO  Fill in with actual models when we have them.
########
@gApp.post("/model/{lModelName}")
def SendUserInput(lModelName: str, lBody: Body):
    lErrorCode, lResult = Models.HandleModelRequest(lModelName, lBody.user_input)

    # Send back a reply depending on the error, if any.
    if lErrorCode == Models.ErrorCodes.SUCCESS:
        return {"reply": lResult}
    elif lErrorCode == Models.ErrorCodes.MODEL_NOT_FOUND:
        raise HTTPException(status_code=404, detail=lResult)


########
# Main entry point.
########
if __name__ == "__main__":
    uvicorn.run("Main:gApp", reload=True)
