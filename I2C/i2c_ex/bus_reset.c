while(!I2C_DATA){
          int counter_clock = 0;
          I2CEnable( DS3231_I2C_BUS, FALSE );  
          I2C_CLOCK_tris = 0;
       
            while(!I2C_DATA && counter_clock++ <10){
                I2C_CLOCK = 0;
                Delayms(1);
                I2C_CLOCK = 1;   
                Delayms(1);
            }

            if(I2C_DATA){
                I2CEnable(DS3231_I2C_BUS, TRUE);
                I2CStop(DS3231_I2C_BUS); // Send the Stop signal  
            }

            I2CEnable(DS3231_I2C_BUS, FALSE);
            reset_ds3231(500);                                     //Desliga e Liga o DS3231
            I2CSetFrequency(DS3231_I2C_BUS, GetPeripheralClock(), I2C_CLOCK_FREQ);
            I2CEnable( DS3231_I2C_BUS, TRUE );
     }