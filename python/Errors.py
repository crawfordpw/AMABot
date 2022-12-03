#####################################################################
##
## Errors.py
##
## Contains error codes the server could generate.
##
#####################################################################

from enum import Enum

#=======#
# Enum of possible errors the server can generate.
#=======#
class ErrorCodes(Enum):
    SUCCESS           = 0
    MODEL_NOT_FOUND   = 1
    MODEL_NOT_RUNNING = 2
    TOO_MANY_CHARS    = 3
