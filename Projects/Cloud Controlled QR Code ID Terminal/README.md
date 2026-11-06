# QR Code Access Control System

A low-cost, automated identification system that reduces human intervention in controlling access to exam halls and buildings. Built around QR code passes, an Arduino, and a cloud-based management site. Designed for Low- and Middle-Income Country (LMIC) settings, with Ahmadu Bello University (ABU), Zaria CBT centers as the case study.

> Status: working prototype. See [Limitations](#limitations) and [Future Work](#future-work).


---

## Table of Contents

- [Problem and Goals](#problem-and-goals)
- [Key Features](#key-features)
- [How It Works](#how-it-works)
- [System Architecture](#system-architecture)
- [Hardware](#hardware)
- [Software Stack](#software-stack)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Design Decisions](#design-decisions)
- [Limitations](#limitations)
- [Future Work](#future-work)
- [Team and Context](#team-and-context)
- [Contact](#contact)

---

## Problem and Goals

Access to exam halls and buildings often depends on people manually checking passes. Automated identification already exists in developed economies, but it tends to be too expensive to adopt elsewhere.

This project aims to be a practical alternative, built around three goals:

| Goal | What it means here |
|---|---|
| Affordability | Uses cheap, widely available parts and free software libraries |
| Reusability | The same hardware and software can serve any venue or event |
| Mass reproducibility | Unique IDs and passes can be generated in bulk at near-zero cost |

## Key Features

- Unique ID generation in bulk, converted to QR codes printed on disposable passes
- Admin website to register students and manage their IDs
- Per-pass scan limits: the admin can cap how many times a pass may be scanned (for example, 2 scans means the third is denied)
- Cloud-based lookup, so the microcontroller does not need to store any records
- Name and course displayed on the Arduino-connected display when a valid pass is scanned

## How It Works

1. The admin registers a student on the website and the system assigns a unique ID.
2. The ID is converted to a QR code and printed on a disposable pass, together with the student's credentials and the exams they are taking.
3. At the entrance, the pass is scanned. A camera captures the frame, OpenCV handles the image and PyZbar decodes the QR code into a string.
4. The decoded string is sent to the cloud, which searches for a match and checks the scan limit.
5. If a match is found and the scan limit has not been reached, the result is sent to the Arduino over USB serial at 9600 baud.
6. The display shows the student's name and course. If there is no match or the limit is exceeded, access is denied.

```
QR pass --> Camera --> OpenCV + PyZbar --> Cloud lookup (website/database)
                                                  |
                                    match / limit check
                                                  |
                              USB serial, 9600 baud --> Arduino --> Display
```

## System Architecture

| Component | Role |
|---|---|
| QR pass | Carries the unique ID as a QR code |
| PC or phone camera | Captures the image of the pass |
| Python scanner script | Captures frames (OpenCV), decodes the QR code (PyZbar), queries the cloud, forwards the result over serial |
| Management website and database | Stores registered students and IDs, generates QR codes, enforces scan limits |
| Arduino | Receives the result over serial and drives the display |



## Hardware

- Arduino Uno (ATmega328P: 32KB flash, 2KB RAM)
- Display: 16x2 I2C LCD Display
- PC or phone camera (used in the current prototype)
- USB cable for the serial link
- Planned: GM803-L scanner module (640x480 CMOS, UART/USB, decodes QR codes on board). It did not arrive in time, so the PC camera pipeline was used instead.

### Wiring



## Software Stack

- Arduino C/C++ (firmware)
- Python 3 with OpenCV and PyZbar (scanner script)
- Firebase for database and html, css and javascript for the site

## Getting Started

### Prerequisites

- Arduino IDE
- Python 3.x
- A camera (laptop webcam or phone camera feed)
- ZBar library, needed by PyZbar. On Linux: `sudo apt install libzbar0`. The Windows pip package bundles it.

### Installation

```bash
git clone https://github.com/Fadlullah04/Embedded-Systems.git
cd Projects/

pip install opencv-python pyzbar pyserial
# TODO: or: pip install -r requirements.txt
```

### Arduino

1. Open the sketch in the Arduino IDE. 
2. Select your board and port, then upload.


## Usage

1. Start the management site and register a student. 
2. Generate and print the student's QR pass.
3. Connect the Arduino and run the scanner script:

```bash
python scannerfile.py 
```

4. Hold the pass up to the camera. The display shows the student's name and course on a match.

## Project Structure

```
.
├── skech/skeck.ino        # Arduino firmware
├── scannerfile.py/        # Python QR scanner (OpenCV + PyZbar)
|__ logicfile.py/          # Scanner logic and website tunnel
├── index.html/        # Management site and database
├── docs/           # report
└── README.md
```

## Design Decisions

- **Why QR codes:** they allow unique IDs to be generated in bulk and printed at almost no cost. QR codes use Reed-Solomon error correction with four levels (L, M, Q, H) that can recover roughly 7%, 15%, 25% and 30% of damaged data. This keeps passes readable even when folded or smudged. Higher levels trade away data capacity.
- **Why the cloud:** the Arduino Uno has very little memory (32KB flash, 2KB RAM), so student records and scan counts live on the server and the microcontroller only displays results.
- **Why a PC camera pipeline:** the dedicated scanner module was delayed in delivery, so image capture and decoding moved to software (OpenCV and PyZbar).

## Limitations

- **Needs internet access:** the lookup is cloud-based. A WiFi-capable board such as the ESP32 would help, as would an offline fallback.
- **Dependence on a PC camera:** this can add delay. A dedicated scanner module such as the GM65 or GM66 would be faster and more reliable.
- **No physical actuator:** the system displays a result but does not yet open or lock a door.

## Future Work

- Move to an ESP32 to remove the PC from the loop
- Add a dedicated QR scanner module (GM65, GM66 or the GM803-L)
- Add a relay or solenoid lock so the result physically controls the door
- Pilot at an ABU CBT center and gather feedback from the university

## Team and Context

Built as a group mini-project for a semester embedded systems assessment at Ahmadu Bello University, Zaria.
- Abdurrahman Muhammad - Lead
- Khalifa Abdulmalik
- Doose Abigail Tsegba
- Ridwan Bolaji
- Abubakar Sadiq

## Contact

Fadlullah: [(https://www.linkedin.com/in/abdurrahman-muhammad-kabir-9293873a8/?lipi=urn%3Ali%3Apage%3Ad_flagship3_profile_view_base%3BzZvcerYdQOipBSKCGXVkZg%3D%3D)]
