
import lcddriver

from time import *



lcd = lcddriver.lcd()

lcd.lcd_clear()

#while True:
#  lcd.lcd_display_string("Need a help",1)
#  lcd.lcd_display_string("At next stop",2)

  
while True:

	now = localtime()

	dt = "%04d-%02d-%02d" % (now.tm_year, now.tm_mon, now.tm_mday)

	tt = "%02d:%02d:%02d" % (now.tm_hour, now.tm_min, now.tm_sec)

	lcd.lcd_display_string(dt, 1)

	lcd.lcd_display_string(tt, 2)

	sleep(1) 
