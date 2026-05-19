Aura: Audio-Visual Response Assistant

Aura is an embedded hardware system built using the Arduino platform. The project processes voice commands to deliver synchronized text and audio outputs, functioning as a localized hardware assistant.

System Architecture and Working Flow:
Voice Input -> Arduino Processes Command -> LCD Displays Answer -> DFPlayer Plays Audio Response

Hardware Integration and Specifications:

Microcontroller: Arduino Mega 2560 handles the primary processing logic and data pin mapping.

Display Interface: A TFT LCD is connected to the Arduino Mega using an 8-bit parallel interface (data pins D0 to D7) along with control lines (CS, RS, WR, RD) for rendering text.

Audio Module: A DFPlayer Mini is integrated via TX/RX lines for serial communication to trigger specific audio tracks.

Sound Output: A speaker is connected directly to the DFPlayer outputs for real-time audio playback.

Power Management: A standard breadboard is utilized for centralized power distribution across all peripheral modules.
