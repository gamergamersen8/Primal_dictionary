import tkinter as tk
from numpy import nan
import pandas as pd
from matplotlib import font_manager
from sys import platform

if platform == 'linux' or platform == 'linux2':
    print("running on linux")
    fontpath = "usr/share/fonts"
elif platform == 'darwin':
    print("running on apple os")
    fontpath = "System/Library/AssetsV2"

elif platform == 'win32':
    print("running on windows")
    fontpath = "C:\\Windows\\Fonts"

else:
    print("unable to locate operating system")
fmanager = font_manager.FontManager()

primal_found = False
primal_path = "PRIMALF.TTF"

font_dir = font_manager.findSystemFonts
for font in font_manager.findSystemFonts(fontpaths=fontpath, fontext='ttf'):
    if font == 'PRIMALF.TTF' or font == "Primalfont Regular":
        primal_found = True


if not primal_found:
    print("primalfont wasn't found on device")
    try:
        fmanager.addfont(primal_path)

    except FileNotFoundError:
        print(
            "unable to find PRIMALF.ttf" + "\n" + "For full functionality please download it from \nhttps://drive.google.com/drive/folders/1ZZSTmk-vP8bgne08j_ajIfp0eGVJhuoq?usp=sharing")

df = pd.read_excel("Skyhawk Primal Reformat.xls", 1, na_values=nan)

bilingual_dict = {}
english_word = df.columns[0]
grammatical_type = df.columns[1]
primal_word = df.columns[3]
pronunciation = df.columns[4]
Skyhawk_pronunciation = df.columns[5]
grouping_type = df.columns[6]
grouping_type_2 = df.columns[7]

bilingual_dict[english_word] = {
    'english word': english_word,
    'translation': primal_word,
    'pronunciation': pronunciation,
    'Skyhawks pronunciation': Skyhawk_pronunciation,
    'grammatical type': grammatical_type,
    'grouping': grouping_type
}

root = tk.Tk()
text_box = tk.Text(root, wrap='word', height=10, width=50)


class Translations:

    def __init__(self, rt, tb):
        self.root = rt
        self.root.title("English to Primal")

        self.text_box = tb

        self.entry = tk.Entry(self.root, font=("Times New Roman", 14))
        self.entry.pack(pady=10)

        translate_button = tk.Button(
            self.root,
            text="Translate",
            command=lambda: [self.clear_text(),
                             self.translate()],
            font=("Times New Roman", 14))
        translate_button.pack(pady=10)
        self.Exit = tk.Button(root,
                              text="Clear Tabs",
                              command=self.clear_text,
                              font=("Times New Roman", 14))
        self.Exit.pack(pady=10)

    def custom_font(self, message, highlight):
        self.text_box = tk.Text(self.root, wrap='word', height=10, width=50)
        self.text_box.pack(padx=100, pady=100)
        start_index = self.text_box.index('end-1c')
        # text_box.insert('end', highlighted)
        end_index = self.text_box.index('end')

        if type(highlight) is not int:
            self.text_box.insert(tk.END, message)

        elif highlight > 1:
            self.text_box.insert('end', "Possible words \n\n", 'Times New Roman')
            for i in range(message.shape[0]):
                primal = message.iloc[i, 3]
                english = message.iloc[i, 0]
                gramma = message.iloc[i, 1]

                # print(primal, english, gramma)
                self.text_box.insert(tk.END, f"The word {english} is  ",
                                     'Times New Roman')
                self.text_box.insert(tk.END, f"{primal} ", 'Primalfont')
                self.text_box.insert(tk.END, f"and it's a {gramma} \n\n", 'Times New Roman')

        else:
            trick = df.at[message, pronunciation]
            skyhawks = df.at[message, Skyhawk_pronunciation]
            gramma = df.at[message, grammatical_type]
            grou = {df.at[message, grouping_type_2]}
            primal = df.at[message, primal_word]

            trickster_translation = f"\n\nFollowing trickster's pronunciation it's {trick}"
            skyhawks_translation = f"\nFollowing Skyhawk's pronunciation it's {skyhawks}"
            grammatical = f"\n\nIt's a {gramma}"

            self.text_box.insert('end', "It's written: ", "Times New Roman")
            self.text_box.insert('end', primal, "Primalfont")
            self.text_box.tag_config("Primalfont", font=("Primalfont", 14))
            self.text_box.insert('end', trickster_translation, "Times New Roman")
            self.text_box.insert('end', skyhawks_translation, "Times New Roman")
            self.text_box.insert('end', grammatical, "Times New Roman")

            if grou != {'nan'} or grou != {nan} or grou.empty:
                grouping = f"\nit's in the following groups {grou}"
                self.text_box.insert('end', grouping, "Times New Roman")
        try:
            self.text_box.tag_config("Primalfont", font=(fmanager.FontEntry("PRIMALF.TTF"), 14))
        except AttributeError:
            self.text_box.tag_config("Primalfont", font=("Primalfont", 12))
        self.text_box.tag_config("Times New Roman", font=("Times New Roman", 28))

    def translate(self):
        word_to_check = self.entry.get()
        matches = None

        if word_to_check in df[english_word].values:
            row_number = (df.index[df[english_word] == word_to_check].tolist()[0])
            matches = 1

        if matches is None:
            matches = df[df[english_word].str.contains(word_to_check,
                                                       case=False,
                                                       na=False)]

        try:
            if not matches.empty:
                print(matches)
                self.custom_font(matches, matches.shape[0])
                return "Sent"
            else:
                self.custom_font(
                    f"The word '{word_to_check}' was not found check spelling, write in simplest form and capital letter at names",
                    "Error")
                return False
        except AttributeError:
            self.custom_font(row_number, 1)
            return True

    def clear_text(self):
        if self.text_box.get("1.0", "end-1c").strip():
            self.text_box.destroy()


app = Translations(root, text_box)
root.mainloop()
