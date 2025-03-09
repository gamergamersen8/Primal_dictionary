import tkinter as tk
import pandas as pd
from numpy import isnan

df = pd.read_excel("Skyhawk Primal Reformat.xls", 1)

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


def custom_font(message, highlight):

    text_box = tk.Text(root, wrap='word', height=10, width=50)
    text_box.pack(padx=100, pady=100)

    if highlight > 1:
        text_box.insert('end', "Possible words \n", 'Times New Roman')
        rows = message.shape[0]
        for i in range(rows):
            text_box.insert('end', f"{message.at[i, english_word]} is in primal: ", 'Times New Roman')
            text_box.insert('end', message.at[i, primal_word], 'Primalfont')
            text_box.insert('end', f"\n\nFollowing Trickster's pronunciation it's {message.at[i, pronunciation]}", 'Times New Roman')
            text_box.insert('end', f"\nFollowing Skyhawk's pronunciation it's {message.at[i, Skyhawk_pronunciation]}", 'Times New Roman')
            text_box.insert('end', f"\nIt's a {message.at[i, grammatical_type]}", 'Times New Roman')
            text_box.insert('end', f"\n\nIt's in the following groups {message.at[i, grouping_type], message.at[i, grouping_type_2]}", 'Times New Roman')

    elif type(message) == int:
        trick = df.at[message, pronunciation]
        skyhawks = df.at[message, Skyhawk_pronunciation]
        gramma = df.at[message, grammatical_type]
        grou = {df.at[message, grouping_type_2]}
        primal = df.at[message, primal_word]

        trickster_translation = f"\n\nFollowing trickster's pronunciation it's {trick}"
        skyhawks_translation = f"\nFollowing Skyhawk's pronunciation it's {skyhawks}"
        grammatical = f"\n\nIt's a {gramma}"

        text_box.insert('end', "It's written: ", "Times New Roman")
        text_box.insert('end', primal, "Primalfont")
        text_box.insert('end', trickster_translation, "Times New Roman")
        text_box.insert('end', skyhawks_translation, "Times New Roman")
        text_box.insert('end', grammatical, "Times New Roman")

        if not isnan(grou[0]):
            grouping = f"\nit's in the following groups {grou}"
            text_box.insert('end', grouping, "Times New Roman")

    start_index = text_box.index('end-1c')
    # text_box.insert('end', highlighted)
    end_index = text_box.index('end')
    text_box.tag_config("Primalfont", font=("Primalfont", 14))
    text_box.tag_config("Times New Roman", font=("Times New Roman", 22))

    text_box.tag_add("highlight", start_index, end_index)


def translate():
    word_to_check = entry.get()
    matches = None

    if word_to_check in df[english_word].values:
        row_number = (df.index[df[english_word] == word_to_check].tolist()[0])
        matches = 1

    if matches is None:
        matches = df[df[english_word].str.contains(word_to_check, case=False, na=False)]

    print(f"The amount of matches found {matches}")

    try:
        if not matches.empty:
            custom_font(matches, len(matches))
        else:
            custom_font(f"The word '{word_to_check}' was not found check spelling, write in simplest form and capital letter at names", "Error")
    except AttributeError:
        custom_font(row_number, 1)
    else:
        custom_font(row_number, 1)


root.title("English to primal")

# Create an entry widget for user input
entry = tk.Entry(root, font=("Times New Roman", 14))
entry.pack(pady=10)

# Create a button to trigger the translation
translate_button = tk.Button(root, text="Translate", command=translate, font=("Times New Roman", 14))
translate_button.pack(pady=10)

# Create a label to display the result
result_label = tk.Label(root, text="", font=("Comic Sans MS", 20))  # Unique font
result_label.pack(pady=10)

# Run the application
root.mainloop()
