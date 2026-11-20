import cv2
import mediapipe as mp
import socket
from RPLCD.i2c import CharLCD

# --- ESP32 WI-FI CONFIGURATION ---
ESP32_IP = "10.72.132.29"
UDP_PORT = 4210
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# --- I2C LCD CONFIGURATION ---
# Address is usually 0x27 or 0x3F. Port=1 is the default I2C bus on Jetson Nano.
lcd = CharLCD(i2c_expander='PCF8574', address=0x27, port=1, cols=16, rows=2, charmap='A02')
lcd.clear()

# --- MEDIAPIPE INITIALIZATION ---
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=2, min_detection_confidence=0.7, min_tracking_confidence=0.7)
mp_draw = mp.solutions.drawing_utils

def compter_doigts(hand_landmarks):
    bouts_doigts = [8, 12, 16, 20]
    articulations = [6, 10, 14, 18]
    doigts_leves = 0

    if hand_landmarks.landmark[4].x < hand_landmarks.landmark[3].x:
        doigts_leves += 1

    for i in range(4):
        if hand_landmarks.landmark[bouts_doigts[i]].y < hand_landmarks.landmark[articulations[i]].y:
            doigts_leves += 1

    return doigts_leves

# --- CAMERA START ---
cap = cv2.VideoCapture(0 + cv2.CAP_V4L2)
print("Camera started. 9 LEDs, 7 Segments, and LCD mode active...")

dernier_chiffre = -1 # State variable to prevent LCD flickering

while True:
    success, img = cap.read()
    if not success:
        break

    img = cv2.flip(img, 1)
    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    results = hands.process(img_rgb)

    total_doigts = 0

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            total_doigts += compter_doigts(hand_landmarks)
            mp_draw.draw_landmarks(img, hand_landmarks, mp_hands.HAND_CONNECTIONS)

    chiffre_a_afficher = min(total_doigts, 9)

    # Send UDP command to the ESP32
    message = f"C:{chiffre_a_afficher}"
    sock.sendto(message.encode(), (ESP32_IP, UDP_PORT))

    # --- UPDATE LCD SCREEN ---
    if chiffre_a_afficher != dernier_chiffre:
        leds_allumees = chiffre_a_afficher
        leds_eteintes = 9 - chiffre_a_afficher

        lcd.clear()
        lcd.cursor_pos = (0, 0)
        lcd.write_string(f"Leds allume: {leds_allumees}")
        lcd.cursor_pos = (1, 0)
        lcd.write_string(f"Leds eteinte: {leds_eteintes}")

        dernier_chiffre = chiffre_a_afficher

    cv2.putText(img, f"Doigts detectes : {chiffre_a_afficher}", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 255, 0), 3)

    cv2.imshow("Vision Jetson", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Clean shutdown
lcd.clear()
cap.release()
cv2.destroyAllWindows()