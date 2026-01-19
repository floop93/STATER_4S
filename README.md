# STATER 4S - Simple sprayer boom leveling system
it's based on 4 ultrasonic sensors (JSN-SR04T). 2 on the each side. And it is dedicated to one piece booms (without ability to move one side separately up and down). 
Because of lack of proportional valve on sprayer it uses impulse method to control boom center of gravity over time. It doesn't act very fast but try to maintain proper angle on slopes.

I used STM Cube IDE to build project (based on HALL libraries) To fit it into machine You need to adjust impulse time and time between them. (all settings can be changed in content.c file, in headers section on top)

DISCLAIMER: Project was made as low budget 'helper' on hills and it doesn't relief operator from paying attention od machine.

This upload is done few years after i made this. It require lot of work to do (if I find some time), Most important things that I'm planning to add is:
  method to adjust impulse timing from device (LCD maybe) 
  boom angle and height sensors. (to add 2 separate wing mode)
  test radar sensors instead of ultrasonic (now its affortable so it can be much improvement) 
  



