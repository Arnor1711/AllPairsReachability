file_name = "origGraph"
new_file_name = "reducedNLGraph"

endTime = 1104537600  # 2005

with open(file_name, "r") as origFile:
    with open(new_file_name, "w") as newFile:

        newFile.write(origFile.readline())
        newFile.write(origFile.readline())

        minTime = 1028243088
        maxTime = 1028243088

        for lineStr in origFile:
            splitted_line = lineStr.split(" ")

            currentTime = int(splitted_line[3])

            if currentTime < endTime:
                newFile.write(lineStr)
