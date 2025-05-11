import serial
import time
import tkinter as tk
from PIL import Image, ImageTk

# Define known UIDs and user data
users = {
    "53156B30": {
        "name": "Ahmed Hassan",
        "roll": "CS-22-104",
        "university": "FAST NUCES CFD",
        "photo": "53156B30.jpg"
    },
    "035964F7": {
        "name": "Raja Bilal",
        "roll": "CS-22-117",
        "university": "FAST NUCES CFD",
        "photo": "035964F7.jpg"
    },
    "F389260E": {
        "name": "Ali Ahmed",
        "roll": "CS-22-099",
        "university": "FAST NUCES CFD",
        "photo": "F389260E.jpg"
    }
}

# Show identity screen
def show_identity(user):
    root = tk.Tk()
    root.title("User Identity")
    root.geometry("400x500")
    root.configure(bg="white")

    try:
        img = Image.open(user['photo'])
        img = img.resize((200, 200))
        photo = ImageTk.PhotoImage(img)
        lbl_img = tk.Label(root, image=photo)
        lbl_img.image = photo
        lbl_img.pack(pady=10)
    except:
        tk.Label(root, text="Image Not Found", fg="red").pack(pady=10)

    tk.Label(root, text=f"Name: {user['name']}", font=("Arial", 16)).pack(pady=5)
    tk.Label(root, text=f"Roll No: {user['roll']}", font=("Arial", 14)).pack(pady=5)
    tk.Label(root, text=f"University: {user['university']}", font=("Arial", 14)).pack(pady=5)

    root.mainloop()

# Serial Setup — Adjust COM port or ttyUSBx accordingly
ser = serial.Serial('/dev/ttyACM0', 115200)  # On Linux, use /dev/ttyUSB0

print("Listening for RFID cards...")

while True:
    line = ser.readline().decode().strip()
    if line.startswith("UID:"):
        uid = line[4:].upper()
        print(f"Scanned UID: {uid}")
        if uid in users:
            show_identity(users[uid])
        else:
            print("Unknown card")
