import cv2
import serial
import time
import numpy as np

arduino = serial.Serial('COM5', 115200, timeout=1)
time.sleep(2)

hand_cascade = cv2.CascadeClassifier("hand.xml")
cap = cv2.VideoCapture(0)

smoothed = 90             
alpha = 0.15          
deadzone = 3                

last_send = time.time()

last_box = None
miss_frames = 0
max_miss = 5   


while True:
    ret, frame = cap.read()
    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.equalizeHist(gray)  
    blur = cv2.GaussianBlur(gray, (5,5), 0)


    hands = hand_cascade.detectMultiScale(
        gray,
        scaleFactor = 1.05,   
        minNeighbors = 3,     
        minSize = (40, 40),   
        flags = cv2.CASCADE_SCALE_IMAGE
    )


    if len(hands) > 0:
        x,y,w,h = max(hands, key=lambda r:r[2]*r[3])
        cx = x + w//2

        frame_w = frame.shape[1]
        raw = np.interp(cx, [0, frame_w], [0, 180])

        gain = 1.6  
        center = 90

        raw = center + (raw - center) * gain

        angle = int(np.clip(raw, 0, 180))


        width = frame.shape[1]
        raw = int(np.interp(cx, [0,width], [0,180]))

        smoothed = int(smoothed * (1-alpha) + raw * alpha)

        if abs(smoothed - raw) < deadzone:
            raw = smoothed

        if time.time() - last_send > 0.04:
            arduino.write(f"{smoothed}\n".encode())
            last_send = time.time()

        cv2.rectangle(frame,(x,y),(x+w,y+h),(0,255,0),2)
        cv2.putText(frame,f"Angle:{smoothed}",(10,30),
                    cv2.FONT_HERSHEY_SIMPLEX,1,(0,255,0),2)

    cv2.imshow("Hand Tracker", frame)
    if cv2.waitKey(1) == 27:
        break

parent_hwnd = None
if len(sys.argv) > 1:
    parent_hwnd = int(sys.argv[1], 16)

cv2.namedWindow("Hand Tracker", cv2.WINDOW_NORMAL)

# Tempelkan ke jendela C++ jika ada parent_hwnd
if parent_hwnd:
    hwnd_opencv = win32gui.FindWindow(None, "Hand Tracker")
    win32gui.SetParent(hwnd_opencv, parent_hwnd)
    # Atur posisi sesuai trackRect C++: { 40, 40, 360, 220 }
    # x=44, y=44 (ditambah padding agar pas di dalam border)
    win32gui.SetWindowPos(hwnd_opencv, None, 44, 44, 312, 172, win32con.SWP_NOZORDER)
    # Hilangkan border jendela OpenCV agar terlihat menyatu
    style = win32gui.GetWindowLong(hwnd_opencv, win32con.GWL_STYLE)
    win32gui.SetWindowLong(hwnd_opencv, win32con.GWL_STYLE, style & ~win32con.WS_CAPTION & ~win32con.WS_THICKFRAME)

while True:
    ret, frame = cap.read()
    if not ret: break
    
    # ... (proses deteksi Anda) ...

    cv2.imshow("Hand Tracker", frame)
    if cv2.waitKey(1) == 27: break


cap.release()
cv2.destroyAllWindows()
arduino.close()
