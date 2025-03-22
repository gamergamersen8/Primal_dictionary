import pandas as pd
from pathlib import Path

if Path("Skyhawk Primal To English.csv").is_file() and Path("Skyhawk English To Primal.csv").is_file():
    print("Files already exist")
else:
    sheet_one = pd.read_excel("Skyhawk Primal Reformat.xls", 0)
    sheet_two = pd.read_excel("Skyhawk Primal Reformat.xls", 1)
    
    sheet_one.to_csv("Skyhawk Primal To English.csv", encoding='utf-8', index=False)
    sheet_two.to_csv("Skyhawk English To Primal.csv", encoding='utf-8', index=False)
