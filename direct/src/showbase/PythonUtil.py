import types
import re

def ifAbsentPut(dict, key, newValue):
    """
    If dict has key, return the value, otherwise insert the newValue and return it
    """
    if dict.has_key(key):
        return dict[key]
    else:
        dict[key] = newValue
        return newValue


def indent(stream, numIndents, str):
    """
    Write str to stream with numIndents in front it it
    """
    #indentString = '\t'
    # To match emacs, instead of a tab character we will use 4 spaces
    indentString = '    '
    stream.write(indentString * numIndents + str)

def apropos(obj, str = None, fOverloaded = 0, width = None,
            fTruncate = 1, lineWidth = 75):
    if type(obj) == types.InstanceType:
        print "***************************INSTANCE INFO*****************************"
    dict = obj.__dict__
    if width:
        maxWidth = width
    else:
        maxWidth = 10
    keyWidth = 0
    aproposKeys = []
    privateKeys = []
    overloadedKeys = []
    remainingKeys = []
    for key in dict.keys():
        if not width:
            keyWidth = len(key)
        if str:
            if re.search(str, key, re.I):
                aproposKeys.append(key)
                if (not width) & (keyWidth > maxWidth):
                    maxWidth = keyWidth
        else:
            if key[:2] == '__':
                privateKeys.append(key)
                if (not width) & (keyWidth > maxWidth):
                    maxWidth = keyWidth
            elif (key[:10] == 'overloaded'):
                if fOverloaded:
                    overloadedKeys.append(key)
                    if (not width) & (keyWidth > maxWidth):
                        maxWidth = keyWidth
            else:
                remainingKeys.append(key)
                if (not width) & (keyWidth > maxWidth):
                    maxWidth = keyWidth
    # Sort appropriate keys
    if str:
        aproposKeys.sort()
    else:
        privateKeys.sort()
        remainingKeys.sort()
        overloadedKeys.sort()
    # Print out results
    keys = aproposKeys + privateKeys + remainingKeys + overloadedKeys
    format = '%-' + `maxWidth` + 's'
    for key in keys:
        value = `dict[key]`
        if fTruncate:
            # Cut off line (keeping at least 1 char)
            value = value[:max(1,lineWidth - maxWidth)]
        print (format % key)[:maxWidth] + '\t' + value
    if type(obj) == types.InstanceType:
        print "*****************************CLASS INFO******************************"
        apropos(obj.__class__, str = str )

def aproposAll(obj):
    apropos(obj, fOverloaded = 1, fTruncate = 0)
    

