import cv2
from pyzbar.pyzbar import decode
import serial
import firebase_admin
from firebase_admin import credentials, db
import time

# 1. Initialize Firebase Connection
# Replace with the actual path to your downloaded .json key and database URL
cred = credentials.Certificate("serviceAccountKey.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://id-admin-df300-default-rtdb.firebaseio.com/'
})

# 2. Connect to the Arduino
# Change 'COM3' to match your Arduino's port (e.g., '/dev/ttyUSB0' on Linux/Mac)
try:
    arduino = serial.Serial('COM15', 9600, timeout=1)
    time.sleep(2) # Give the serial connection time to establish
    print("Connected to Arduino successfully.")
except Exception as e:
    print(f"Failed to connect to Arduino: {e}")
    arduino = None

# 3. Start OpenCV Video Capture
cap = cv2.VideoCapture(0)
print("Starting scanner...")

last_scan_time = 0
cooldown = 3  # Prevent spamming the database with the same QR code

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Decode any QR codes in the frame
    for barcode in decode(frame):
        qr_data = barcode.data.decode('utf-8').strip()
        current_time = time.time()

        if current_time - last_scan_time > cooldown:
            print(f"Scanned: {qr_data}")
            last_scan_time = current_time
            
            # 4. Check the Database
            user_ref = db.reference(f'users/{qr_data}')
            user_data = user_ref.get()

            # 5. Send Signal to Arduino
            if user_data and user_data.get('is_active'):
                print(f"Access Granted for {user_data.get('name')}")
                if arduino:
                    arduino.write(b'1') # Send success byte
            else:
                print("Access Denied")
                if arduino:
                    arduino.write(b'0') # Send failure byte

    # Display the camera feed (optional, good for debugging)
    cv2.imshow('Ad-hoc QR Scanner', frame)

    # Press 'q' to quit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
if arduino:
    arduino.close()