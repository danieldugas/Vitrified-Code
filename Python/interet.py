items = {'oui', 'non'}
the_choice = 0
while the_choice == 0:
    for i in range(30):
        print("\r\n")
    choice = input("Interessé? \r\n\r\n\r\n\r\n\r\n\r\n\r\n ********* \r\n\r\nVotre choix: ")
    if choice in items:
        the_choice = choice
    else:
        print("Je ne parle pas cette langue, barbare")

f = open("C:\install.res.1028.dll", 'r', errors="replace")
for line in f:
    print(line.encode("utf-8", errors="replace"))
for i in range(1,100):
    print("\r\n")

if the_choice == 'oui':
    print("Calcul terminé: Il est indiqué de 'checker out' la chose en question")
else :
    print("Calcul supplémentaire nécessaire...")
    input()
    for line in f:
        print(line.encode("utf-8", errors="replace"))
    for i in range(1,100):
        print("\r\n")

    print("Calcul terminé: Il est indiqué de NE PAS 'checker out' la chose en question")

f.close()

input()
