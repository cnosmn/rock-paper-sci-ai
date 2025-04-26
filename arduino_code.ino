#include <Servo.h>

// Servo motorları tanımlayalım
Servo gozYanal;    // Gözün sağa-sola hareketi için servo
Servo gozDikey;    // Gözün yukarı-aşağı hareketi için servo
Servo boyunYanal;  // Boyunun sağa-sola hareketi için servo
Servo boyunDikey;  // Boyunun yukarı-aşağı hareketi için servo
Servo cene;        // Çene hareketi için servo

// Servo motorların bağlı olduğu pinler
// Bu kısımda değişiklik yapabilirsiniz
const int gozYanalPin = 9;
const int gozDikeyPin = 10;
const int boyunYanalPin = 11;
const int boyunDikeyPin = 12;
const int cenePin = 13;

// Servo açıları - Bu değerleri değiştirerek farklı hareketler elde edebilirsiniz
// Min, orta ve max değerler - her servo için sınırları buradan değiştirebilirsiniz
const int gozYanalMin = 45;
const int gozYanalOrta = 90;
const int gozYanalMax = 135;

const int gozDikeyMin = 45;
const int gozDikeyOrta = 90;
const int gozDikeyMax = 135;

const int boyunYanalMin = 30;
const int boyunYanalOrta = 90;
const int boyunYanalMax = 150;

const int boyunDikeyMin = 70;
const int boyunDikeyOrta = 90;
const int boyunDikeyMax = 110;

const int ceneKapali = 50;
const int ceneYariAcik = 90;
const int ceneAcik = 130;

// Servo hareketlerinin hızı (bekleme süresi - ms)
// Değeri azaltarak daha hızlı, arttırarak daha yavaş hareketler elde edebilirsiniz
const int hareketHizi = 15;

void setup() {
  // Seri haberleşmeyi başlat
  Serial.begin(9600);
  Serial.println("Robot Kafa Kontrol Programı");
  Serial.println("----------------------------");
  Serial.println("Komutlar:");
  Serial.println("1: Temel hareketleri göster");
  Serial.println("2: Göz kırp");
  Serial.println("3: Sağa sola bak");
  Serial.println("4: Yukarı aşağı bak");
  Serial.println("5: Konuşma taklidi yap");
  Serial.println("6: Evet der gibi hareket et");
  Serial.println("7: Hayır der gibi hareket et");
  Serial.println("8: Tüm hareketleri sırayla yap");
  Serial.println("----------------------------");
  
  // Servoları pinlere bağla
  gozYanal.attach(gozYanalPin);
  gozDikey.attach(gozDikeyPin);
  boyunYanal.attach(boyunYanalPin);
  boyunDikey.attach(boyunDikeyPin);
  cene.attach(cenePin);
  
  // Tüm servoları merkez konuma getir
  tumServolarMerkezKonum();
  
  Serial.println("Robot kafa hazır!");
}

void loop() {
  // Seri port üzerinden komut geldi mi diye kontrol et
  if (Serial.available() > 0) {
    char komut = Serial.read();
    
    switch (komut) {
      case '1':
        temelHareketler();
        break;
      case '2':
        gozKirp();
        break;
      case '3':
        sagaSolaBak();
        break;
      case '4':
        yukariAsagiBak();
        break;
      case '5':
        konusmaTaklidi();
        break;
      case '6':
        evetHareketi();
        break;
      case '7':
        hayirHareketi();
        break;
      case '8':
        tumHareketler();
        break;
      default:
        // Tanınmayan bir komut geldiyse merkeze dön
        tumServolarMerkezKonum();
        break;
    }
  }
}

// Tüm servoları merkez konuma getiren fonksiyon
void tumServolarMerkezKonum() {
  Serial.println("Tüm servolar merkez konuma getiriliyor...");
  
  // Yumuşak hareket için kademeli olarak merkeze getir
  yumusakHareket(gozYanal, gozYanal.read(), gozYanalOrta);
  yumusakHareket(gozDikey, gozDikey.read(), gozDikeyOrta);
  yumusakHareket(boyunYanal, boyunYanal.read(), boyunYanalOrta);
  yumusakHareket(boyunDikey, boyunDikey.read(), boyunDikeyOrta);
  yumusakHareket(cene, cene.read(), ceneKapali);
  
  delay(500);
  Serial.println("Merkez konum tamamlandı.");
}

// Yumuşak servo hareketi sağlayan fonksiyon
void yumusakHareket(Servo &servo, int baslangicAci, int bitisAci) {
  // Eğer başlangıç açısı geçersizse, şu anki açıyı kullan
  if (baslangicAci < 0 || baslangicAci > 180) {
    baslangicAci = servo.read();
  }
  
  // Eğer başlangıç ve bitiş açıları aynıysa, hareket etme
  if (baslangicAci == bitisAci) {
    return;
  }
  
  // Servo hareketi için yön belirle
  int adim = (baslangicAci < bitisAci) ? 1 : -1;
  
  // Yumuşak hareket için kademeli olarak ilerle
  for (int aci = baslangicAci; aci != bitisAci; aci += adim) {
    servo.write(aci);
    delay(hareketHizi);
  }
  
  // Son hedef açıya ulaştığından emin ol
  servo.write(bitisAci);
}

// Temel hareketleri gösteren fonksiyon
void temelHareketler() {
  Serial.println("Temel hareketler gösteriliyor...");
  
  // Önce merkeze gel
  tumServolarMerkezKonum();
  delay(1000);
  
  // Gözleri hareket ettir
  yumusakHareket(gozYanal, gozYanalOrta, gozYanalMin);
  delay(500);
  yumusakHareket(gozYanal, gozYanalMin, gozYanalMax);
  delay(500);
  yumusakHareket(gozYanal, gozYanalMax, gozYanalOrta);
  delay(500);
  
  yumusakHareket(gozDikey, gozDikeyOrta, gozDikeyMin);
  delay(500);
  yumusakHareket(gozDikey, gozDikeyMin, gozDikeyMax);
  delay(500);
  yumusakHareket(gozDikey, gozDikeyMax, gozDikeyOrta);
  delay(500);
  
  // Çeneyi hareket ettir
  yumusakHareket(cene, ceneKapali, ceneAcik);
  delay(500);
  yumusakHareket(cene, ceneAcik, ceneKapali);
  delay(500);
  
  // Boynu hareket ettir
  yumusakHareket(boyunYanal, boyunYanalOrta, boyunYanalMin);
  delay(500);
  yumusakHareket(boyunYanal, boyunYanalMin, boyunYanalMax);
  delay(500);
  yumusakHareket(boyunYanal, boyunYanalMax, boyunYanalOrta);
  delay(500);
  
  yumusakHareket(boyunDikey, boyunDikeyOrta, boyunDikeyMin);
  delay(500);
  yumusakHareket(boyunDikey, boyunDikeyMin, boyunDikeyMax);
  delay(500);
  yumusakHareket(boyunDikey, boyunDikeyMax, boyunDikeyOrta);
  delay(500);
  
  Serial.println("Temel hareketler tamamlandı.");
}

// Göz kırpma hareketi
void gozKirp() {
  Serial.println("Göz kırpılıyor...");
  
  // Burada bizim kodumuzda göz kapağı hareketi simüle ediliyor
  // Gerçek bir göz kapağı servonuz varsa, o servoyu kullanabilirsiniz
  
  // Göz kırpma simülasyonu - gözleri hızlıca aşağı indirip kaldır
  yumusakHareket(gozDikey, gozDikeyOrta, gozDikeyMax);
  delay(100);
  yumusakHareket(gozDikey, gozDikeyMax, gozDikeyOrta);
  
  Serial.println("Göz kırpma tamamlandı.");
}

// Sağa sola bakma hareketi
void sagaSolaBak() {
  Serial.println("Sağa sola bakılıyor...");
  
  // Önce gözler sonra boyun hareketi
  yumusakHareket(gozYanal, gozYanalOrta, gozYanalMin);
  delay(300);
  yumusakHareket(boyunYanal, boyunYanalOrta, boyunYanalMin);
  delay(500);
  
  // Merkeze dön
  yumusakHareket(gozYanal, gozYanalMin, gozYanalOrta);
  yumusakHareket(boyunYanal, boyunYanalMin, boyunYanalOrta);
  delay(500);
  
  // Diğer tarafa bak
  yumusakHareket(gozYanal, gozYanalOrta, gozYanalMax);
  delay(300);
  yumusakHareket(boyunYanal, boyunYanalOrta, boyunYanalMax);
  delay(500);
  
  // Merkeze dön
  yumusakHareket(gozYanal, gozYanalMax, gozYanalOrta);
  yumusakHareket(boyunYanal, boyunYanalMax, boyunYanalOrta);
  
  Serial.println("Sağa sola bakma tamamlandı.");
}

// Yukarı aşağı bakma hareketi
void yukariAsagiBak() {
  Serial.println("Yukarı aşağı bakılıyor...");
  
  // Önce gözler sonra boyun hareketi
  yumusakHareket(gozDikey, gozDikeyOrta, gozDikeyMin);
  delay(300);
  yumusakHareket(boyunDikey, boyunDikeyOrta, boyunDikeyMin);
  delay(500);
  
  // Merkeze dön
  yumusakHareket(gozDikey, gozDikeyMin, gozDikeyOrta);
  yumusakHareket(boyunDikey, boyunDikeyMin, boyunDikeyOrta);
  delay(500);
  
  // Aşağı bak
  yumusakHareket(gozDikey, gozDikeyOrta, gozDikeyMax);
  delay(300);
  yumusakHareket(boyunDikey, boyunDikeyOrta, boyunDikeyMax);
  delay(500);
  
  // Merkeze dön
  yumusakHareket(gozDikey, gozDikeyMax, gozDikeyOrta);
  yumusakHareket(boyunDikey, boyunDikeyMax, boyunDikeyOrta);
  
  Serial.println("Yukarı aşağı bakma tamamlandı.");
}

// Konuşma taklidi hareketi
void konusmaTaklidi() {
  Serial.println("Konuşma taklidi yapılıyor...");
  
  // Çene hareketleri ile konuşma simülasyonu
  for (int i = 0; i < 5; i++) {
    yumusakHareket(cene, ceneKapali, ceneYariAcik);
    delay(200);
    yumusakHareket(cene, ceneYariAcik, ceneKapali);
    delay(100);
    yumusakHareket(cene, ceneKapali, ceneAcik);
    delay(200);
    yumusakHareket(cene, ceneAcik, ceneKapali);
    delay(100);
  }
  
  Serial.println("Konuşma taklidi tamamlandı.");
}

// Evet deme hareketi
void evetHareketi() {
  Serial.println("Evet hareketi yapılıyor...");
  
  // Boyun dikey hareketiyle evet deme simülasyonu
  for (int i = 0; i < 3; i++) {
    yumusakHareket(boyunDikey, boyunDikeyOrta, boyunDikeyMin);
    delay(300);
    yumusakHareket(boyunDikey, boyunDikeyMin, boyunDikeyMax);
    delay(300);
  }
  
  // Merkeze dön
  yumusakHareket(boyunDikey, boyunDikeyMax, boyunDikeyOrta);
  
  Serial.println("Evet hareketi tamamlandı.");
}

// Hayır deme hareketi
void hayirHareketi() {
  Serial.println("Hayır hareketi yapılıyor...");
  
  // Boyun yanal hareketiyle hayır deme simülasyonu
  for (int i = 0; i < 3; i++) {
    yumusakHareket(boyunYanal, boyunYanalOrta, boyunYanalMin);
    delay(300);
    yumusakHareket(boyunYanal, boyunYanalMin, boyunYanalMax);
    delay(300);
  }
  
  // Merkeze dön
  yumusakHareket(boyunYanal, boyunYanalMax, boyunYanalOrta);
  
  Serial.println("Hayır hareketi tamamlandı.");
}

// Tüm hareketleri sırayla yapan fonksiyon
void tumHareketler() {
  Serial.println("Tüm hareketler başlatılıyor...");
  
  temelHareketler();
  delay(1000);
  
  gozKirp();
  delay(1000);
  
  sagaSolaBak();
  delay(1000);
  
  yukariAsagiBak();
  delay(1000);
  
  konusmaTaklidi();
  delay(1000);
  
  evetHareketi();
  delay(1000);
  
  hayirHareketi();
  delay(1000);
  
  // Son olarak merkeze dön
  tumServolarMerkezKonum();
  
  Serial.println("Tüm hareketler tamamlandı.");
}
