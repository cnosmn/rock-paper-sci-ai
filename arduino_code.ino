#include <Servo.h>

// Servo motorları tanımlayalım
Servo boyunAna;     // Boyunun ana (merkez) dönüşü için servo
Servo boyunYanal;   // Boyunun sağa-sola hareketi için servo
Servo boyunDikey;   // Boyunun yukarı-aşağı hareketi için servo
Servo cene;         // Çene hareketi için servo
Servo gozDikey;     // Gözün yukarı-aşağı hareketi için servo
Servo gozKapakSag;  // Sağ göz kapağı için servo
Servo gozKapakSol;  // Sol göz kapağı için servo
Servo gozYanal;     // Gözün sağa-sola hareketi için servo

// Servo motorların bağlı olduğu pinler
const int BOYUN_ANA = 8;
const int BOYUN_YANAL = 9;
const int BOYUN_DIKEY = 10;
const int CENE = 11;
const int GOZ_DIKEY = 12;
const int GOZ_KAPAK_SAG = 13;
const int GOZ_KAPAK_SOL = 14;
const int GOZ_YANAL = 15;

// Servo açıları - verilen değerlere göre ayarlandı
// Çene açıları
const int CENE_KAPALI = 70;
const int CENE_YARIM = 90;
const int CENE_ACIK = 110;

// Boyun açıları
const int BOYUN_DIKEY_MIN = 30;    // Minimum açı - öne eğilme
const int BOYUN_DIKEY_MERKEZ = 50; // Merkez açı - nötr pozisyon
const int BOYUN_DIKEY_MAX = 70;    // Maksimum açı - arkaya eğilme

const int BOYUN_YANAL_MIN = 30;
const int BOYUN_YANAL_MERKEZ = 90;
const int BOYUN_YANAL_MAX = 160;

const int BOYUN_ANA_MIN = 10;
const int BOYUN_ANA_MERKEZ = 50;
const int BOYUN_ANA_MAX = 90;

// Göz açıları
const int GOZ_YANAL_MIN = 45;
const int GOZ_YANAL_MERKEZ = 100;
const int GOZ_YANAL_MAX = 135;

const int GOZ_DIKEY_MIN = 55;
const int GOZ_DIKEY_MERKEZ = 80;
const int GOZ_DIKEY_MAX = 100;

// Göz kapak açıları
const int GOZ_KAPAK_SAG_ACIK = 70;
const int GOZ_KAPAK_SAG_KAPALI = 0;
const int GOZ_KAPAK_SOL_ACIK = 110;
const int GOZ_KAPAK_SOL_KAPALI = 180;

// Servo hareketlerinin hızı (bekleme süresi - ms)
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
  Serial.println("9: Servoları merkez konuma getir");
  Serial.println("----------------------------");
  
  // Servoları pinlere bağla
  boyunAna.attach(BOYUN_ANA);
  boyunYanal.attach(BOYUN_YANAL);
  boyunDikey.attach(BOYUN_DIKEY);
  cene.attach(CENE);
  gozDikey.attach(GOZ_DIKEY);
  gozKapakSag.attach(GOZ_KAPAK_SAG);
  gozKapakSol.attach(GOZ_KAPAK_SOL);
  gozYanal.attach(GOZ_YANAL);
  
  // Tüm servoları merkez konuma getir
  servolarMerkezKonum();
  
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
      case '9':
        servolarMerkezKonum();
        break;
      default:
        // Tanınmayan bir komut geldiyse bilgi ver
        Serial.println("Geçersiz komut! 1-9 arası bir rakam girin.");
        break;
    }
  }
}

// Yumuşak servo hareketi sağlayan fonksiyon
void servoYumusakHareket(Servo &servo, int baslangicAci, int bitisAci) {
  // Başlangıç açısı belirtilmediyse mevcut açıyı al
  if (baslangicAci == -1) {
    baslangicAci = servo.read();
  }
  
  // Açı sınırlaması
  if (bitisAci < 0) bitisAci = 0;
  if (bitisAci > 180) bitisAci = 180;
  
  // Başlangıç ve bitiş açıları aynıysa işlem yapma
  if (baslangicAci == bitisAci) {
    return;
  }
  
  // Hareket yönünü belirle (artarak veya azalarak)
  int adim = (baslangicAci < bitisAci) ? 1 : -1;
  
  // Yumuşak hareket için açıyı adım adım değiştir
  for (int aci = baslangicAci; aci != bitisAci; aci += adim) {
    servo.write(aci);
    delay(hareketHizi);  // Her adımda belirtilen süre kadar bekle
  }
  
  // Son açı değerine ulaştığından emin ol
  servo.write(bitisAci);
}

// Tüm servoları merkez konuma getiren fonksiyon
void servolarMerkezKonum() {
  Serial.println("Tüm servolar merkez konuma getiriliyor...");
  
  // Göz kapaklarını aç
  servoYumusakHareket(gozKapakSag, -1, GOZ_KAPAK_SAG_ACIK);
  servoYumusakHareket(gozKapakSol, -1, GOZ_KAPAK_SOL_ACIK);
  
  // Gözleri merkeze getir
  servoYumusakHareket(gozYanal, -1, GOZ_YANAL_MERKEZ);
  servoYumusakHareket(gozDikey, -1, GOZ_DIKEY_MERKEZ);
  
  // Çeneyi kapat
  servoYumusakHareket(cene, -1, CENE_KAPALI);
  
  // Boyun servolarını merkeze getir
  servoYumusakHareket(boyunDikey, -1, BOYUN_DIKEY_MERKEZ);
  servoYumusakHareket(boyunYanal, -1, BOYUN_YANAL_MERKEZ);
  servoYumusakHareket(boyunAna, -1, BOYUN_ANA_MERKEZ);
  
  delay(500);
  Serial.println("Merkez konum tamamlandı.");
}

// Temel hareketleri gösteren fonksiyon
void temelHareketler() {
  Serial.println("Temel hareketler gösteriliyor...");
  
  // Önce merkeze gel
  servolarMerkezKonum();
  delay(1000);
  
  // Boyun Ana servo hareketi
  Serial.println("Boyun ana servo hareketi");
  servoYumusakHareket(boyunAna, BOYUN_ANA_MERKEZ, BOYUN_ANA_MIN);
  delay(500);
  servoYumusakHareket(boyunAna, BOYUN_ANA_MIN, BOYUN_ANA_MAX);
  delay(500);
  servoYumusakHareket(boyunAna, BOYUN_ANA_MAX, BOYUN_ANA_MERKEZ);
  delay(500);
  
  // Boyun Yanal servo hareketi
  Serial.println("Boyun yanal servo hareketi");
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MERKEZ, BOYUN_YANAL_MIN);
  delay(500);
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MIN, BOYUN_YANAL_MAX);
  delay(500);
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MAX, BOYUN_YANAL_MERKEZ);
  delay(500);
  
  // Boyun Dikey servo hareketi
  Serial.println("Boyun dikey servo hareketi");
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MERKEZ, BOYUN_DIKEY_MIN);
  delay(500);
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MIN, BOYUN_DIKEY_MAX);
  delay(500);
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MAX, BOYUN_DIKEY_MERKEZ);
  delay(500);
  
  // Göz Yanal servo hareketi
  Serial.println("Göz yanal servo hareketi");
  servoYumusakHareket(gozYanal, GOZ_YANAL_MERKEZ, GOZ_YANAL_MIN);
  delay(500);
  servoYumusakHareket(gozYanal, GOZ_YANAL_MIN, GOZ_YANAL_MAX);
  delay(500);
  servoYumusakHareket(gozYanal, GOZ_YANAL_MAX, GOZ_YANAL_MERKEZ);
  delay(500);
  
  // Göz Dikey servo hareketi
  Serial.println("Göz dikey servo hareketi");
  servoYumusakHareket(gozDikey, GOZ_DIKEY_MERKEZ, GOZ_DIKEY_MIN);
  delay(500);
  servoYumusakHareket(gozDikey, GOZ_DIKEY_MIN, GOZ_DIKEY_MAX);
  delay(500);
  servoYumusakHareket(gozDikey, GOZ_DIKEY_MAX, GOZ_DIKEY_MERKEZ);
  delay(500);
  
  // Çene servo hareketi
  Serial.println("Çene servo hareketi");
  servoYumusakHareket(cene, CENE_KAPALI, CENE_YARIM);
  delay(500);
  servoYumusakHareket(cene, CENE_YARIM, CENE_ACIK);
  delay(500);
  servoYumusakHareket(cene, CENE_ACIK, CENE_KAPALI);
  delay(500);
  
  Serial.println("Temel hareketler tamamlandı.");
}

// Göz kırpma hareketi
void gozKirp() {
  Serial.println("Göz kırpılıyor...");
  
  // Göz kapakları ile göz kırpma
  // Önce iki göz kapağını da kapat
  servoYumusakHareket(gozKapakSag, GOZ_KAPAK_SAG_ACIK, GOZ_KAPAK_SAG_KAPALI);
  servoYumusakHareket(gozKapakSol, GOZ_KAPAK_SOL_ACIK, GOZ_KAPAK_SOL_KAPALI);
  delay(200);
  
  // Sonra iki göz kapağını da aç
  servoYumusakHareket(gozKapakSag, GOZ_KAPAK_SAG_KAPALI, GOZ_KAPAK_SAG_ACIK);
  servoYumusakHareket(gozKapakSol, GOZ_KAPAK_SOL_KAPALI, GOZ_KAPAK_SOL_ACIK);
  
  delay(500);
  
  // Tek göz kırpma (sadece sağ göz)
  servoYumusakHareket(gozKapakSag, GOZ_KAPAK_SAG_ACIK, GOZ_KAPAK_SAG_KAPALI);
  delay(200);
  servoYumusakHareket(gozKapakSag, GOZ_KAPAK_SAG_KAPALI, GOZ_KAPAK_SAG_ACIK);
  
  Serial.println("Göz kırpma tamamlandı.");
}

// Sağa sola bakma hareketi
void sagaSolaBak() {
  Serial.println("Sağa sola bakılıyor...");
  
  // Önce merkeze gel
  servolarMerkezKonum();
  delay(500);
  
  // Önce gözler sonra boyun hareketi - Sağa
  servoYumusakHareket(gozYanal, GOZ_YANAL_MERKEZ, GOZ_YANAL_MIN);
  delay(300);
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MERKEZ, BOYUN_YANAL_MIN);
  delay(1000);
  
  // Merkeze dön
  servoYumusakHareket(gozYanal, GOZ_YANAL_MIN, GOZ_YANAL_MERKEZ);
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MIN, BOYUN_YANAL_MERKEZ);
  delay(500);
  
  // Sola bak
  servoYumusakHareket(gozYanal, GOZ_YANAL_MERKEZ, GOZ_YANAL_MAX);
  delay(300);
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MERKEZ, BOYUN_YANAL_MAX);
  delay(1000);
  
  // Merkeze dön
  servoYumusakHareket(gozYanal, GOZ_YANAL_MAX, GOZ_YANAL_MERKEZ);
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MAX, BOYUN_YANAL_MERKEZ);
  
  Serial.println("Sağa sola bakma tamamlandı.");
}

// Yukarı aşağı bakma hareketi
void yukariAsagiBak() {
  Serial.println("Yukarı aşağı bakılıyor...");
  
  // Önce merkeze gel
  servolarMerkezKonum();
  delay(500);
  
  // Yukarı bak
  servoYumusakHareket(gozDikey, GOZ_DIKEY_MERKEZ, GOZ_DIKEY_MIN);
  delay(300);
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MERKEZ, BOYUN_DIKEY_MIN);
  delay(1000);
  
  // Merkeze dön
  servoYumusakHareket(gozDikey, GOZ_DIKEY_MIN, GOZ_DIKEY_MERKEZ);
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MIN, BOYUN_DIKEY_MERKEZ);
  delay(500);
  
  // Aşağı bak
  servoYumusakHareket(gozDikey, GOZ_DIKEY_MERKEZ, GOZ_DIKEY_MAX);
  delay(300);
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MERKEZ, BOYUN_DIKEY_MAX);
  delay(1000);
  
  // Merkeze dön
  servoYumusakHareket(gozDikey, GOZ_DIKEY_MAX, GOZ_DIKEY_MERKEZ);
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MAX, BOYUN_DIKEY_MERKEZ);
  
  Serial.println("Yukarı aşağı bakma tamamlandı.");
}

// Konuşma taklidi hareketi
void konusmaTaklidi() {
  Serial.println("Konuşma taklidi yapılıyor...");
  
  // Çene hareketleri ile konuşma simülasyonu
  for (int i = 0; i < 5; i++) {
    servoYumusakHareket(cene, CENE_KAPALI, CENE_YARIM);
    delay(100);
    servoYumusakHareket(cene, CENE_YARIM, CENE_KAPALI);
    delay(120);
    servoYumusakHareket(cene, CENE_KAPALI, CENE_ACIK);
    delay(150);
    servoYumusakHareket(cene, CENE_ACIK, CENE_KAPALI);
    delay(100);
  }
  
  Serial.println("Konuşma taklidi tamamlandı.");
}

// Evet deme hareketi
void evetHareketi() {
  Serial.println("Evet hareketi yapılıyor...");
  
  // Boyun dikey hareketiyle evet deme simülasyonu
  for (int i = 0; i < 3; i++) {
    servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MERKEZ, BOYUN_DIKEY_MIN);
    delay(300);
    servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MIN, BOYUN_DIKEY_MAX);
    delay(300);
  }
  
  // Merkeze dön
  servoYumusakHareket(boyunDikey, BOYUN_DIKEY_MAX, BOYUN_DIKEY_MERKEZ);
  
  Serial.println("Evet hareketi tamamlandı.");
}

// Hayır deme hareketi
void hayirHareketi() {
  Serial.println("Hayır hareketi yapılıyor...");
  
  // Boyun yanal hareketiyle hayır deme simülasyonu
  for (int i = 0; i < 3; i++) {
    servoYumusakHareket(boyunYanal, BOYUN_YANAL_MERKEZ, BOYUN_YANAL_MIN);
    delay(300);
    servoYumusakHareket(boyunYanal, BOYUN_YANAL_MIN, BOYUN_YANAL_MAX);
    delay(300);
  }
  
  // Merkeze dön
  servoYumusakHareket(boyunYanal, BOYUN_YANAL_MAX, BOYUN_YANAL_MERKEZ);
  
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
  servolarMerkezKonum();
  
  Serial.println("Tüm hareketler tamamlandı.");
}
