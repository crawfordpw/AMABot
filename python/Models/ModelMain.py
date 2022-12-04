#####################################################################
##
## ModelMain.py
##
## Contains Model specific classes and functions that every AI model
## will use.
##
#####################################################################

from abc import abstractmethod
from Errors import ErrorCodes

#=======#
# Base class of an AI model. Each model should derive from
# this class and implement their own run method.
#=======#
class Model:
    def __init__(self, lName : str, lCommandName : str, lMaxChars : int):
        self.__mName            = lName             # Name of the model.
        self.__mCommandName     = lCommandName      # Corresponds to command name in Discord (C++).
        self.__mMaxChars        = lMaxChars         # Max input the model can receive.
        self.mIsRunning         = False             # Flag to check if it's running or not.

    #-------#
    # Getter for model name.
    #-------#
    def GetName(self):
        return self.__mName

    #-------#
    # Getter for command name related to this model.
    #-------#
    def GetCommandName(self):
        return self.__mCommandName

    #-------#
    # Is the current current within the max character limit the model could receive.
    # param[in] lLen    Length of the input string.
    #-------#
    def ValidInput(self, lLen : int):
        if lLen > self.__mMaxChars:
            return False
        return True

    #-------#
    # Check before calling the Run method to see if we can/should send it input.
    # param[in] lLen    Length of the input string.
    #-------#
    def CanRun(self, lLen : int):
        if not self.mIsRunning:
             return ErrorCodes.MODEL_NOT_RUNNING, "Sorry, " + self.GetName() + " is not currently running!"
        elif not self.ValidInput(lLen):
            return ErrorCodes.TOO_MANY_CHARS, self.GetName() + " requires a maximum of " + self.__mMaxChars + "characters. You gave " + lLen + " characters."

        return ErrorCodes.SUCCESS, ""

    #-------#
    # This should be overriden by child classes. It's is where we will
    # actually send the user input to and receive the generated output.
    #-------#
    @abstractmethod
    def Run(self, lUserInput : str):
        pass
