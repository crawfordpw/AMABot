#####################################################################
##
## Main.py
##
## Main file for running the REST server.
##
#####################################################################

import uvicorn
import ApiCalls
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

"""
TODO

Add api call:
    - GET current model running
        - This will allow discord to query if model is running
        before trying to send it input.
"""

#
# Globals
#
gReplyAttribute = "reply"
gApp            = FastAPI()

#=======#
# Structure for input coming from Discord.
#=======#
class Body(BaseModel):
    user_input: str

#-------#
# Initialize our program by setting up all
# of our models.
#-------#
@gApp.on_event('startup')
def Init():
    ApiCalls.InitModels()
    return

#-------#
# Landing page? Should this be the ping endpoint instead?
#-------#
@gApp.get("/")
async def root():
    return { gReplyAttribute: "Hello World" }

#-------#
# Used for determining if the server is up or not.
#-------#
@gApp.get("/ping")
def Ping():
    return { gReplyAttribute: "pong" }

#-------#
# Reply back with the current models
# that our server is running.
#-------#
@gApp.get("/running_models")
def GetRunningModels():
    lReply = ""
    for lModel in ApiCalls.gModels:
        if lModel.mIsRunning:
            lReply += lModel.GetInteralName() + " "
    return { gReplyAttribute: lReply.strip() };

#-------#
# Main call for the server. This will send off the user
# input from Discord to the model for generating a reply back.
# TODO  Fill in with actual models when we have them.
#-------#
@gApp.post("/model/{lModelName}")
def SendUserInput(lModelName: str, lBody: Body):
    lErrorCode, lResult = ApiCalls.HandleModelRequest(lModelName, lBody.user_input)

    # Send back a reply depending on the error, if any.
    if lErrorCode == ApiCalls.ErrorCodes.SUCCESS:
        return { gReplyAttribute: lResult }
    elif lErrorCode == ApiCalls.ErrorCodes.MODEL_NOT_FOUND:
        raise HTTPException(status_code=404, detail=lResult)

#########
# Main entry point.
#########
if __name__ == "__main__":
    uvicorn.run("Main:gApp", reload=True)
