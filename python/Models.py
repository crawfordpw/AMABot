#####################################################################
##
## Models.py
##
## Contains class for a Model structure as well as dictionary of
## all available models that can be run.
##
#####################################################################

from enum import Enum
from abc import abstractmethod

gModels = {}

class ErrorCodes(Enum):
    SUCCESS           = 0
    MODEL_NOT_FOUND   = 1
    MODEL_NOT_RUNNING = 2
    TOO_MANY_CHARS    = 3

class Model:
    def __init__(self, lName : str, lInternalName : str, lMaxChars : int):
        self.__mName            = lName
        self.__mInternalName    = lInternalName
        self.__mMaxChars        = lMaxChars
        self.mIsRunning         = False

    def GetName(self):
        return self.__mName

    def GetInternalName(self):
        return self.__mInternalName

    def ValidInput(self, lLen : int):
        if lLen > self.__mMaxChars:
            return False
        return True

    def CanRun(self, lLen : int):
        if not self.mIsRunning:
             return ErrorCodes.MODEL_NOT_RUNNING, "Sorry, " + self.GetName() + " is not currently running!"
        elif not self.ValidInput(lLen):
            return ErrorCodes.TOO_MANY_CHARS, self.GetName() + " requires a maximum of " + self.__mMaxChars + "characters. You gave " + lLen + " characters."

        return ErrorCodes.SUCCESS, ""

    @abstractmethod
    def Run(self, lUserInput : str):
        pass

class TestModel(Model):
    def __init__(self, lMaxChars : int):
        super().__init__("Test", "test", lMaxChars)
        self.mTestString = "But I must explain to you how all this mistaken idea of denouncing pleasure and praising \
            pain was born and I will give you a complete account of the system, and expound the actual teachings of the \
            great explorer of the truth, the master-builder of human happiness. No one rejects, dislikes, or avoids pleasure \
            itself, because it is pleasure, but because those who do not know how to pursue pleasure rationally encounter \
            consequences that are extremely painful. Nor again is there anyone who loves or pursues or desires to obtain \
            pain of itself, because it is pain, but because occasionally circumstances occur in which toil and pain can \
            procure him some great pleasure. To take a trivial example, which of us ever undertakes laborious physical exercise, \
            except to obtain some advantage from it? But who has any right to find fault with a man who chooses to enjoy a pleasure \
            that has no annoying consequences, or one who avoids a pain that produces no resultant pleasure? On the other hand, we \
            denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the \
            moment, so blinded by desire, that they cannot foresee the pain and trouble that are bound to ensue; and equal blame belongs \
            to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. \
            These cases are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled and when \
            nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain \
            circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be \
            repudiated and annoyances accepted. The wise man therefore always holds in these matters to this principle of selection: he rejects \
            pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains. But I must explain to you how al"

    def Run(self, lUserInput : str):
        lErrorCode, lResult = super().CanRun(len(lUserInput))

        if lErrorCode == ErrorCodes.SUCCESS:
            lResult += "FastApi has Received input: " + lUserInput
            lResult += " " + self.mTestString  + self.mTestString

        return lErrorCode, lResult


def InitModels():
    global gModels

    lTestModel = TestModel(200)
    lTestModel.mIsRunning = True

    gModels[lTestModel.GetInternalName()] = lTestModel
    lModel      = gModels.get("test")
    print(lModel.GetName())

def HandleModelRequest(lModelName : str, lUserInput : str):
    global gModels

    lErrorCode  = ErrorCodes.MODEL_NOT_FOUND
    lResult     = "Model not found!"
    lModel      = gModels.get(lModelName)

    if lModel != None:
        print("here!")
        lErrorCode, lResult = lModel.Run(lUserInput)

    return lErrorCode, lResult
