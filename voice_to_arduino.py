import serial
import sounddevice as sd
from scipy.io.wavfile import write
from transformers import pipeline
import pyttsx3
import time


SERIAL_PORT = "COM8"     # Change this to your Arduino port
BAUDRATE = 9600
RECORD_SECONDS = 5
SAMPLE_RATE = 16000
TEMP_WAV = "voice.wav"

print("Loading Whisper model...")
pipe = pipeline("automatic-speech-recognition", model="Shubham09/wispher2")
print("✅ Whisper model loaded successfully")

# Connect to Arduino safely
arduino = None
try:
    arduino = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
    time.sleep(2)
    print(f"✅ Connected to Arduino on {SERIAL_PORT}")
except Exception as e:
    print(f"⚠️ Couldn't connect to Arduino: {e}")

# Setup voice engine
engine = pyttsx3.init()
engine.setProperty('rate', 160)

# --- Keyword dictionary (slang + general) ---
knowledge_base = {
    
    # Gen Z slang
    "idk": "It means I don't know.",
    "brb": "It means be right back.",
    "lol": "It means laughing out loud.",
    "sus": "It means suspicious.",
    "lit": "It means awesome or cool.",
    "bet": "It means okay or deal.",
    "cap": "It means lying.",
    "no cap": "It means no lie.",
    "rizz": "It means charm or charisma.",
    "on god": "It means seriously.",
    "slay": "It means you did great.",
    "fam": "It means close friend.",
    "gyat": "It means strong reaction.",
    "skibidi": "It’s just a funny meme word.",
    "idc": "It means I don’t care.",
    "fr": "It means for real.",
    "drip": "It means stylish clothes or fashion.",
    "vibe": "Vibe means mood or energy.",

    # General
    "name": "I am your Gen Z voice assistant.",
    "how are you": "I’m vibing and doing great!",
    "who made you": "You did! I’m your creation.",
    "ai": "AI means Artificial Intelligence. It helps computers think smart.",
    "python": "Python is a programming language that’s easy and powerful.",
    "arduino": "Arduino is a microcontroller used for electronics projects.",
    "gen z": "Gen Z is the generation born between 1997 and 2012.",
    "time": lambda: time.strftime("It's %I:%M %p right now."),
}

# --- Helper functions ---
def record_audio():
    """Records short audio clip from mic"""
    print("🎙️ Listening... Speak now!")
    try:
        data = sd.rec(int(RECORD_SECONDS * SAMPLE_RATE), samplerate=SAMPLE_RATE, channels=1, dtype='int16')
        sd.wait()
        write(TEMP_WAV, SAMPLE_RATE, data)
        print("✅ Audio recorded")
        return TEMP_WAV
    except Exception as e:
        print("⚠️ Recording error:", e)
        return None

def transcribe_audio(path):
    """Transcribes audio to text"""
    try:
        result = pipe(path)
        text = result['text'].strip().lower()
        print(f"🧠 You said: {text}")
        return text
    except Exception as e:
        print("⚠️ Transcription failed:", e)
        return ""

def get_response(text):
    """Finds best response from dictionary"""
    for key, value in knowledge_base.items():
        if key in text:
            return value() if callable(value) else value
    return "Hmm, I’m not sure about that, but sounds cool!"

def speak_and_send(response):
    """Speaks out loud and sends to Arduino (if available)"""
    print("💬 Response:", response)
    try:
        engine.say(response)
        engine.runAndWait()
    except Exception as e:
        print("⚠️ TTS error:", e)
    if arduino:
        try:
            arduino.write((response + "\n").encode('utf-8'))
            print("📤 Sent to Arduino successfully")
        except Exception as e:
            print("⚠️ Serial write error:", e)

# --- Main loop ---
print("\n🎧 Voice Assistant Ready — press Enter to start talking\n")
while True:
    try:
        input("👉 Press Enter to record (Ctrl+C to exit): ")

        if arduino:
            arduino.write(b"*")  # optional: signal Arduino that recording starts

        wav = record_audio()
        if not wav:
            continue

        text = transcribe_audio(wav)
        if not text:
            continue

        response = get_response(text)
        speak_and_send(response)

        time.sleep(1)

    except KeyboardInterrupt:
        print("\n👋 Exiting program...")
        if arduino:
            arduino.close()
        break
