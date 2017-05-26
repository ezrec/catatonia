import sys

def readin(name=""):
    f=open("/flash/" + name, "w")
    while True:
        l = sys.stdin.readline()
        if l.startswith("EOF"):
            f.close()
            break
        f.write(l)
