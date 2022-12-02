#####################################################################
##
## Server.py
##
## Main file for running the REST server.
##
#####################################################################

import time
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

"""
TODO

Add api call:
    - GET current model running
        - This will allow discord to query if model is running
        before trying to send it input.

Add Main entry point of server.
"""

########
# This string is used to test really long output messages for the
# bot and should be removed.
########
lTestString = "But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was born and I will give you a complete account of the system, and expound the actual teachings of the great explorer of the truth, the master-builder of human happiness. No one rejects, dislikes, or avoids pleasure itself, because it is pleasure, but because those who do not know how to pursue pleasure rationally encounter consequences that are extremely painful. Nor again is there anyone who loves or pursues or desires to obtain pain of itself, because it is pain, but because occasionally circumstances occur in which toil and pain can procure him some great pleasure. To take a trivial example, which of us ever undertakes laborious physical exercise, except to obtain some advantage from it? But who has any right to find fault with a man who chooses to enjoy a pleasure that has no annoying consequences, or one who avoids a pain that produces no resultant pleasure? On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. These cases are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise man therefore always holds in these matters to this principle of selection: he rejects pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains. But I must explain to you how al"

########
# Thinking this should be a dictionary of all possible models, where
# the value is a boolean of whether model is up or not. This can then
# be used to look up when someone queries for a specific model
########
gModel = "test";

## Our FastAPI application.
gApp = FastAPI()

########
# Structure for input coming from Discord.
########
class Body(BaseModel):
    mUserInput: str


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
    return {"reply": gModel};

########
# Main call for the server. This will send off the user
# input from Discord to the model for generating a reply back.
# TODO  Fill in with actual models when we have them.
########
@gApp.post("/model/{lModelName}")
def HandleModelRequest(lModelName: str, lBody: Body):
    if lModelName == "test":
        lString = "FastApi has Received input: " + lBody.mUserInput
        #time.sleep(5)
        lString += " " + lTestString + lTestString
        return {"reply": lString}

    raise HTTPException(status_code=404, detail="Model not found!")


########
# TODO Probably add a main function here to start and
# run Uvicorn.
########
