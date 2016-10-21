import smtplib

DATA_FOLDER = "/home/daniel/"
with open(DATA_FOLDER+"secret.txt") as file:
        secret=file.read().replace('\n', '') 

server = smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()
server.login("brainwavesdev@gmail.com", secret)

msg = "Brains waved!"
server.sendmail("brainwavesdev@gmail.com", "brainwavesdev@gmail.com", msg)
server.quit()
