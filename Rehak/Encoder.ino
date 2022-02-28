void Encoder() 
{
 buttonState = encoder.getButton();

  if (buttonState != 0){
     scale.tare();
     switch (buttonState){
       case ClickEncoder::Open:          //0
         break;
       case ClickEncoder::Closed:        //1
         break;
       case ClickEncoder::Pressed:       //2
         break;
       case ClickEncoder::Held:          //3
         break;
       case ClickEncoder::Released:      //4
         break;
       case ClickEncoder::Clicked:       //5
         break;
       case ClickEncoder::DoubleClicked: //6
         break;
     }
   }
}
