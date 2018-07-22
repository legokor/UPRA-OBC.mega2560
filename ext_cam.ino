void ecam_init(void)
{
  DEBUG.print(F("OBC: INIT EXTERNAL CAMERA..."));

  pinMode(MENU, OUTPUT);
  digitalWrite(MENU, LOW);
  pinMode(SHTR, INPUT);
  digitalWrite(SHTR, LOW);

  DEBUG.println(F("OK"));

}

void ecam_pressSHTR(int duration)
{
  DEBUG.print(F("OBC: ECAM SHUTTER..."));
  pinMode(SHTR, OUTPUT);
  digitalWrite(SHTR, HIGH);
  delay(duration);
  digitalWrite(SHTR, LOW);
  pinMode(SHTR, INPUT);
  DEBUG.println(F("PRESSED"));
}

void ecam_pressMENU(int duration)
{
  DEBUG.print(F("OBC: ECAM MENU..."));
  digitalWrite(MENU, HIGH);
  delay(duration);
  digitalWrite(MENU, LOW);
  DEBUG.println(F("PRESSED"));
}


void ecam_ON(void)
{
  ecam_pressMENU(100);
  is_ecam_on = true;
  DEBUG.println(F("OBC: ECAM ON"));
}

void ecam_OFF(void)
{
  ecam_pressMENU(1000);
  is_ecam_on = false;
  DEBUG.println(F("OBC: ECAM OFF"));
}

void ecam_VideoMode(void)
{
  ecam_pressMENU(100);
  delay(500);  
  ecam_pressMENU(100);
  delay(500);  
  ecam_pressMENU(100);
  delay(500);  
}

void ecam_PictureMode(void)
{
  ecam_pressMENU(100);
  delay(500);  
}


