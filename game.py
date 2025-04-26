import cv2
import random
import serial
import time
import mediapipe as mp
import pyttsx3


class RockPaperScissorsGame:
    def __init__(self, serial_port='COM3', baud_rate=115200, timer_interval=5):
        self.serial_conn = serial.Serial(serial_port, baud_rate)
        self.timer_interval = timer_interval

        self.cap = cv2.VideoCapture(0)
        self.hands = mp.solutions.hands.Hands(static_image_mode=False,
                                              max_num_hands=1,
                                              min_detection_confidence=0.7)
        self.mp_draw = mp.solutions.drawing_utils

        # Skorlar
        self.user_score = 0
        self.robot_score = 0
        self.draw_count = 0

        # Sesli geri bildirim
        self.engine = pyttsx3.init()
        self.engine.setProperty('rate', 150)

    def speak(self, text):
        self.engine.say(text)
        self.engine.runAndWait()

    def predict_hand_gesture(self, hand_landmarks):
        finger_tips = [8, 12, 16, 20]
        open_fingers = sum(
            hand_landmarks.landmark[tip_id].y < hand_landmarks.landmark[tip_id - 2].y
            for tip_id in finger_tips
        )

        if open_fingers == 0:
            return "tas"
        elif open_fingers == 4:
            return "kagit"
        elif open_fingers == 2:
            return "makas"
        return "belirsiz"

    def get_user_move(self):
        start_time = time.time()
        predicted_move = "belirsiz"

        while time.time() - start_time < self.timer_interval:
            success, frame = self.cap.read()
            if not success:
                continue

            frame = cv2.flip(frame, 1)
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            result = self.hands.process(rgb_frame)

            if result.multi_hand_landmarks:
                for hand_landmarks in result.multi_hand_landmarks:
                    predicted_move = self.predict_hand_gesture(hand_landmarks)
                    self.mp_draw.draw_landmarks(frame, hand_landmarks, mp.solutions.hands.HAND_CONNECTIONS)

            countdown = max(0, self.timer_interval - int(time.time() - start_time))
            cv2.putText(frame, f"Eliniz: {predicted_move}", (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 255, 0), 2)
            cv2.putText(frame, f"Sure: {countdown}", (10, 80), cv2.FONT_HERSHEY_SIMPLEX, 1.0, (255, 255, 0), 2)
            cv2.putText(frame, f"Skor - Siz: {self.user_score} Robot: {self.robot_score} Beraberlik: {self.draw_count}",
                        (10, 460), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (200, 200, 255), 2)

            cv2.imshow("Tas-Kagit-Makas Oyunu", frame)
            if cv2.waitKey(1) & 0xFF == 27:
                self.cleanup()
                exit()

        return predicted_move

    def get_random_robot_move(self):
        return random.choice(["tas", "kagit", "makas"])

    def send_move_to_arduino(self, move):
        if self.serial_conn.is_open:
            self.serial_conn.write((move + "\n").encode())

    def determine_winner(self, user_move, robot_move):
        if user_move == robot_move:
            self.draw_count += 1
            return "Beraberlik!"
        elif (user_move == "tas" and robot_move == "makas") or \
             (user_move == "kagit" and robot_move == "tas") or \
             (user_move == "makas" and robot_move == "kagit"):
            self.user_score += 1
            return "Tebrikler, kazandınız!"
        else:
            self.robot_score += 1
            return "Üzgünüm, robot kazandı."

    def play_round(self):
        print("Yeni tur başlıyor...")
        user_move = self.get_user_move()
        robot_move = self.get_random_robot_move()

        print(f"İnsan: {user_move} | Robot: {robot_move}")
        result_text = self.determine_winner(user_move, robot_move)

        print("Sonuç:", result_text)
        self.speak(result_text)
        self.send_move_to_arduino(robot_move)
        time.sleep(3)

    def cleanup(self):
        self.cap.release()
        cv2.destroyAllWindows()
        if self.serial_conn.is_open:
            self.serial_conn.close()

    def start(self):
        print("Taş-Kağıt-Makas Oyunu Başladı (Çıkmak için ESC tuşuna basın)")
        try:
            while True:
                self.play_round()
        except KeyboardInterrupt:
            print("\nOyun durduruldu.")
        finally:
            self.cleanup()


if __name__ == "__main__":
    game = RockPaperScissorsGame(serial_port='/dev/ttyUSB0', baud_rate=115200, timer_interval=5)
    game.start()
