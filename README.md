# MAX32630FTHR-Wearable-EKG
My first extensive foray into embedded systems. A wearable ekg system designed using Maxim's MAX32630FTHR development board and the Texas Instruments ADS1291 EKG analog front end. This project was created using ARM's mbed online ide.

This project will remain as is as documentation, but may be continued in another repo at a future date.

2024-06-14:
In case the [All About Circuits blog post](https://forum.allaboutcircuits.com/ubs/max32630fthr-wearable-ekg.990) or [competion](https://www.allaboutcircuits.com/giveaways/get-creative-makewithmaxim-design-contest/) go down (and to keep everything in one place): 

Introduction
In order to showcase the capabilities of the MAX32630FTHR it seemed like the best idea would create a wearable sensor that demonstrates its ability to run on a battery as well as its bluetooth capabilities and SD card slot. With my background as a student in biomedical engineering, it felt like a wearable EKG unit would be the perfect choice. This system will record analog EKG data from the patient, store the data on an SD card for future review, and transmit the data to the patient's smartphone to be viewed in real time.

BOM
MAX32630FTHR
3.5mm audio jack
Electrodes
3.5mm compatible electrode leads
HC-05 (optional)
ADS1291 EKG Analog Front-End
3.7V LiPo battery


Schematics

Instructions
Using the schematics above created in DigiKey's Schemeit software, wire together the analog front-end ic, the Pegasus board, as well as the HC-06 module in absence of bluetooth compatible firmware.

It should look something like this:
![image](https://github.com/tylermccollum/MAX32630FTHR-Wearable-EKG/assets/25785478/9d15e0e0-ded8-4d33-a4f3-ab04359f7c5f)

(Electrode leads not pictured, resistors should be between the EKG leads and analog inputs for safety)

Connect the electrodes to your body in Lead I configuration (RA = AIN+, LA = AIN-, LL = Ground). Ignore color coding in image below, it's only for location reference.

![image](https://github.com/tylermccollum/MAX32630FTHR-Wearable-EKG/assets/25785478/cca2b53d-c88c-4325-bd1c-03a5dd0f01d6)

After uploading the firmware and starting the system, the data should be able to be viewed on the Bluetooth Graphics app available from the Google Play app marketplace. Alternatively, one could create their own app, however this was chosen due to time constraints and ease of use.

The end result should appear similar to as follows:

![image](https://github.com/tylermccollum/MAX32630FTHR-Wearable-EKG/assets/25785478/87a206db-3174-4e5f-a41c-9512a738665c)

![image](https://github.com/tylermccollum/MAX32630FTHR-Wearable-EKG/assets/25785478/48e3a46d-ec00-4be0-a245-715eef21e91f)



Video
Coming soon!

Source Code
All the code for this project is hosted at: https://github.com/tylermccollum/MAX32630FTHR-Wearable-EKG

Future Direction
Were I to continue with this project, future directions include full onboard bluetooth functionality, signal filtering, and specific app software.

#MakeWithMaxim
