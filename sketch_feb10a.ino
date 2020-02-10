struct pinset {
  int A;
  int B;
};

struct fn {
  String type;
  pinset pin;
};

bool parseLine(fn *cmd, String line);

const pinset NONE = {0, 0};
const pinset PIN_A = {2, 3};
const pinset PIN_B = {4, 5};
pinset current = NONE;

bool isNone(pinset p) {
  return (p.A == NONE.A && p.B == NONE.B);
}

void set(pinset p) {
  Serial.print("execute set(");
  Serial.print(p.A);
  Serial.print(",");
  Serial.print(p.B);
  Serial.println(")");
  digitalWrite(p.A, HIGH);
  digitalWrite(p.B, HIGH);
}

void reset(pinset p) {
  Serial.print("execute reset(");
  Serial.print(p.A);
  Serial.print(",");
  Serial.print(p.B);
  Serial.println(")");
  digitalWrite(p.A, LOW);
  digitalWrite(p.B, LOW);
}

void outputMode(pinset p) {
  pinMode(p.A, OUTPUT);
  pinMode(p.B, OUTPUT);
}

void setup() {
  // put your setup code here, to run once:
  outputMode(PIN_A);
  outputMode(PIN_B);
  reset(PIN_A);
  reset(PIN_B);
  Serial.begin(115200);
  Serial.println("USB Switcher was booted!!");
  Serial.println("You can switch USB port by typing <command> via USB-Serial port.");
  Serial.println("  Example: set 1");
  Serial.println();
  Serial.println("You can use commands are listed below");
  Serial.println("  set <number>\\n   :   Plug to USB port corresponding to <number>");
  Serial.println("  reset <number>\\n : Unplug to USB port corresponding to <number>");
  Serial.println();
  Serial.println("<number>:");
  Serial.println("  0 : Nothing");
  Serial.println("  1 : USB port A");
  Serial.println("  2 : USB port B");
  Serial.println();
  Serial.println("If you send a command 'set 0' or 'reset 0', current connected USB port is unplugged.");
}

bool parseLine(fn *cmd, String line) {
  line.toLowerCase();
  if(line.startsWith("set") || line.startsWith("reset"))
  {
    int idx = line.indexOf(' ');
    if(idx < 0) {
      return false;
    }
    
    String substr = line.substring(idx, line.length());
    int pinnum = substr.toInt();
    if(pinnum < 0 || 3 <= pinnum) {
      return false;
    }

    cmd->type = line.substring(0, idx);
    if(pinnum == 0) {
      cmd->pin = NONE;
      return true;
    }
    if(pinnum == 1) {
      cmd->pin = PIN_A;
      return true;
    }
    if(pinnum == 2) {
      cmd->pin = PIN_B;
      return true;
    }
    
    return false;
  }

  return false;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    fn cmd;
    bool ret = parseLine(&cmd, line);
    if(!ret) {
      Serial.println("parsing was failed");
      return;
    }

    if(!isNone(current)) {
      reset(current);
    }

    if(cmd.type.startsWith("set")) {
      if(!isNone(cmd.pin)) {
        set(cmd.pin);
      }
      
      current = cmd.pin;
      return;
    }

    if(cmd.type.startsWith("reset")) {
      if(!isNone(cmd.pin)) {
        reset(cmd.pin);
      }
      return;
    }

    Serial.println("invalid command");
  }
  delay(100);
}
