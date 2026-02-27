import sys
import json
import os

def CookShader(slangPath, jsonPath):
    shaderData = {
        "Shader": os.path.basename(slangPath).replace(".slang", ".spv"),
        "Passes": {}
    }

    currentPass = None

    with open(slangPath, 'r') as file:
        for line in file:
            line = line.strip()

            if line.startswith("import") or line.startswith("[shader"):
                break

            if line.startswith("//$"):
                parts = line[3:].split(maxsplit=1)
                if len(parts) < 2: continue
                
                key, value = parts[0], parts[1]

                if key == "Pass":
                    currentPass = value
                    shaderData["Passes"][currentPass] = {}
                elif currentPass:
                    if value.lower() == "true" : value = True
                    elif value.lower() == "false": value = False

                    shaderData["Passes"][currentPass][key] = value

    with open(jsonPath, 'w') as outFile:
        json.dump(shaderData, outFile, indent=4)

    print(f"Successfully cooked {jsonPath}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python ShaderCooker.py <input.slang> <output.json>")
        sys.exit(1)
        
    CookShader(sys.argv[1], sys.argv[2])
   
