import os
from dotenv import load_dotenv

load_dotenv()
github_token = os.getenv("GITHUB_TOKEN")

if github_token:
    env.Append(CPPDEFINES=[("GITHUB_TOKEN", '\\"' + github_token + '\\"')])
else:
    print("Warning: GITHUB_TOKEN not found in .env")

