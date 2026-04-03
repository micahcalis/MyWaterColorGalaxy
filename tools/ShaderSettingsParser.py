import sys
import json
import os

def CookShader(slangPath, jsonPath):
    normalizedPath = slangPath.lower().replace('\\', '/')
    isCompute = "compute" in normalizedPath

    shaderData = {
        "Shader": os.path.basename(slangPath).replace(".slang", ".spv")
    }

    if isCompute:
        shaderData["Kernels"] = {}
        currentKernel = None
        kernelIndex = 0

        with open(slangPath, 'r') as file:
            for line in file:
                line = line.strip()

                # 1. Look for the Kernel definition (e.g., //$ Kernel Perlin)
                if line.startswith("//$ Kernel"):
                    parts = line[3:].split(maxsplit=1)
                    if len(parts) >= 2:
                        currentKernel = parts[1]
                        shaderData["Kernels"][currentKernel] = {
                            "Kernel": kernelIndex,
                            "Compute": currentKernel # <--- Directly assigning the tag name here!
                        }
                        kernelIndex += 1
                    continue
                
                # 2. Extract manual tags
                if currentKernel and line.startswith("//$"):
                    parts = line[3:].split(maxsplit=1)
                    if len(parts) >= 2:
                        key, value = parts[0], parts[1]
                        if value.lower() == "true": value = True
                        elif value.lower() == "false": value = False
                        elif value.isdigit(): value = int(value)
                        
                        shaderData["Kernels"][currentKernel][key] = value
                    continue

    else:
        # --- ORIGINAL GRAPHICS SHADER LOGIC ---
        shaderData["Passes"] = {}
        currentPass = None

        with open(slangPath, 'r') as file:
            for line in file:
                line = line.strip()

                if line.startswith("[shader"):
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