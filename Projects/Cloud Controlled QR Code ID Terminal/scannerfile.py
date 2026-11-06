import time
import os
import cv2
import serial
import firebase_admin
from firebase_admin import credentials, db

script_dir = os.path.dirname(os.path.abspath(__file__))

# Build the exact path to the JSON file
key_path = os.path.join(script_dir, "serviceAccountKey.json")

# 1. Initialize Firebase Admin SDK using the new dynamic path
cred = credentials.Certificate(key_path)
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://id-admin-df300-default-rtdb.firebaseio.com/' 
})

# 2. Setup Serial Connection to Arduino (Adjust COM port for Windows or /dev/ttyUSB for Linux)
SERIAL_PORT = 'COM15'  
BAUD_RATE = 9600

try:
    arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)  # Allow Arduino time to reset after serial connection
    print(f"[+] Connected to Arduino on {SERIAL_PORT}")
except Exception as e:
    print(f"[-] Serial connection failed: {e}")
    arduino = None

# 3. Initialize OpenCV QR Code Detector and Webcam
cap = cv2.VideoCapture(0)
detector = cv2.QRCodeDetector()

last_scanned = ""
last_scan_time = 0
COOLDOWN_SECONDS = 3  # Prevent spamming the database/serial connection

print("[+] Access Control Scanner running... Press 'q' to quit.")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Detect and decode QR code from the camera frame
    data, bbox, _ = detector.detectAndDecode(frame)

    current_time = time.time()
    if data and (data != last_scanned or (current_time - last_scan_time) > COOLDOWN_SECONDS):
        last_scanned = data
        last_scan_time = current_time
        print(f"\n[SCAN] Scanned QR Code: {data}")

        # Query Firebase directly at path: students/<scanned_code>
        ref = db.reference(f'students/{data}')
        student_data = ref.get()

        if student_data:
            student_name = student_data.get('name', 'Student')
            student_track = student_data.get('department', 'No Department') # Fetch the department # Fetch the track
            print(f"[ACCESS GRANTED] Welcome, {student_name}")
            
            # Send format '1|Name|Track\n' to Arduino
            if arduino:
                payload = f"1|{student_name}|{student_track}\n"
                arduino.write(payload.encode('utf-8'))
        else:
            print("[ACCESS DENIED] Unregistered QR Code")
            
            # Send '0\n' to Arduino for Access Denied
            if arduino:
                arduino.write(b"0\n")

    # Visual feedback on camera window
    cv2.putText(frame, "Align QR Code within frame", (20, 40), 
                cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    cv2.imshow("Access Control Scanner", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
if arduino:
    arduino.close()