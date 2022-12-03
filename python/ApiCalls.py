#####################################################################
##
## ApiCalls.py
##
## Functions that the server will call for further processing of
## the REST requests.
##
#####################################################################

from Errors import ErrorCodes
from Models.TestModel import TestModel

# Dictionary of model classes
gModels = {}

#-------#
# Create our dictionary of models and initialize them.
#-------#
def InitModels():
    global gModels

    lTestModel = TestModel(200)
    lTestModel.mIsRunning = True

    gModels[lTestModel.GetInternalName()] = lTestModel

#-------#
# Once we receive a request to generate some AI output,
# send it to the model it it exists.
#
# param[in] lModelName    Model that Discord is requesting.
# param[in] lUserInput    Input string the user sent.
#-------#
def HandleModelRequest(lModelName : str, lUserInput : str):
    global gModels

    lErrorCode  = ErrorCodes.MODEL_NOT_FOUND
    lResult     = "Model not found!"
    lModel      = gModels.get(lModelName)

    if lModel != None:
        lErrorCode, lResult = lModel.Run(lUserInput)

    return lErrorCode, lResult
