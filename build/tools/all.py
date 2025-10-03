#
# all.py - Скрипт, который запускает сначала систему сборки, потом запускает бинарник.
#


# Импортируем:
import os
import sys


# Если этот файл запускают:
if __name__ == "__main__":
    if sys.platform == "win32":
        os.system("cls && build.bat && run.bat")
    else: os.system("clear && bash build.sh && bash run.sh")
