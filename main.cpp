#include <iostream>  // Für Ein- und Ausgabe (cout, cin)
#include <fstream>   // Für Dateioperationen (ifstream, ofstream)
#include <vector>    // Für die Nutzung von Vektoren
#include <iomanip>   // Für die Formatierungsausgaben (z.B. setw, hex)
#include <sstream>   // Für die Verarbeitung von Strings mit Streams
#include <string>    // Für die Verwendung von std::string
#include <cctype>    // Für Zeichenoperationen (z.B. isdigit, isalpha)
#include <bitset>    // Für die Umwandlung von Binär-Strings in Ganzzahlen

using namespace std;

void printTable(const vector<unsigned char>& data) {
    const size_t bytesPerRow = 8;  // Anzahl Bytes pro Zeile (anpassbar)
    size_t dataSize = data.size();

    // Vor der Kopfzeile: Füllzeichen auf Leerzeichen setzen
    cout << setfill(' ');

    // Kopfzeile: Fest definierte Breiten für die Spalten
    // Für den Offset reservieren wir 8 Zeichen
    // HEX-Bereich benötigt 8×3= 24 Zeichen, BIN-Bereich 8×9= 72 Zeichen plus 1 Zeichen für Ausrichtung des Tabellenkopfs, Breite des CHAR-Bereichs bleibt flexibel
    cout << "-----------------------------------------------------------------------------------------------------------------------------\n";
    cout << setw(8) << left << "Offset" << " | "
         << setw(24) << left << "HEX (2-stellig Hex)" << " | "
         << setw(73) << left << "BIN (8-stellig Binär)" << " | "
         << "CHAR (ASCII)" << "\n";
    cout << "-----------------------------------------------------------------------------------------------------------------------------\n";

    for (size_t offset = 0; offset < dataSize; offset += bytesPerRow) {
        // Offset ausgeben (in Hexadezimal, 8 Zeichen breit)
        cout << setw(8) << setfill('0') << hex << offset << " | ";
        
        // HEX-Spalte: Für jedes Byte 2-stellige HEX-Zahl plus Leerzeichen (3 Zeichen pro Byte)
        for (size_t i = 0; i < bytesPerRow; ++i) {
            if (offset + i < dataSize) {
                cout << setw(2) << setfill('0') << hex << static_cast<int>(data[offset + i]) << " ";
            } else {
                cout << "   ";
            }
        }
        cout << " | ";

        // BIN-Spalte: Für jedes Byte 8 Zeichen (bitset<8>) plus Leerzeichen (9 Zeichen pro Byte)
        for (size_t i = 0; i < bytesPerRow; ++i) {
            if (offset + i < dataSize) {
                cout << bitset<8>(data[offset + i]) << " ";
            } else {
                cout << "         ";
            }
        }
        cout << " | ";

        // CHAR-Spalte: Druckbare Zeichen oder Punkt
        for (size_t i = 0; i < bytesPerRow; ++i) {
            if (offset + i < dataSize) {
                unsigned char ch = data[offset + i];
                cout << (isprint(ch) ? static_cast<char>(ch) : '.');
            }
        }
        cout << "\n";
    }
    cout << dec << "\n";  // Zurück zur Dezimaldarstellung
}



// Wandelt einen 8-stelligen Binär-String in eine Ganzzahl vom Typ 'unsigned char' um
bool binaryStringToByte(const string& binStr, unsigned char &result) {
    if (binStr.length() != 8)
        return false;
    for (char c : binStr) {
        if (c != '0' && c != '1')
            return false;
    }
    result = static_cast<unsigned char>(bitset<8>(binStr).to_ulong());
    return true;
}

int main(int argc, char* argv[]) {
    string filename;  // Variable zur Speicherung des Dateinamens

    // Prüft, ob ein Argument (Dateipfad) über die Kommandozeile übergeben wurde 
    if (argc >= 2) {
        filename = argv[1];  // Falls ja, wird der Dateipfad aus dem Argument gelesen
    } else {
        // Falls kein Argument übergeben wurde, wird der Benutzer aufgefordert, den Dateipfad einzugeben
        cout << "Bitte Dateipfad eingeben: ";
        getline(cin, filename);  // Einlesen des Dateipfads
    }

    // Öffnen der Datei im Binärmodus
    ifstream inFile(filename, ios::binary);

    // Prüfen, ob die Datei geöffnet werden konnte
    if (!inFile) {
        cerr << "Fehler beim Öffnen der Datei: " << filename << endl;  // Fehlermeldung ausgeben
        return 1;  // Programm mit Fehlercode beenden
    }
    //  Einlesen des Dateiinhalts in einen Vektor
    vector<unsigned char> data((istreambuf_iterator<char>(inFile)),
                                istreambuf_iterator<char>());

    // Schließen der Datei, da der Inhalt bereits eingelesen wurde                            
    inFile.close();

    bool exitLoop = false;  // Steuert die Schleife, solange der Benutzer das Programm nicht beendet
    while (!exitLoop) {
        // Anzeigen der Dateiinhalten in Tabellenform
        cout << "\nAktuelle Dateiinhalte:\n";
        printTable(data);  // Ruft eine Funktion auf, um die Dateiinhalte darzustellen

        // Zeigt dem Benutzer die verfügbaren Optionen an
        cout << "\nOptionen:\n";
        cout << "1. Bearbeite einen Wert\n";
        cout << "2. Speichern und beenden\n";
        cout << "3. Beenden ohne zu speichern\n";
        cout << "Wähle eine Option (1-3): ";

        int option;
        cin >> option;  // Einlesen der Benutzereingabe
        cin.ignore();   // Ignorieren des Zeilenumbruchs

        switch (option) {
            case 1: {  // Fall 1: Ein Wert in der Datei soll bearbeitet werden
                size_t offset;  // Variable zur Speicherung der gewünschten Position (Offset) in der Datei

                // Der Benutzer wird aufgefordert, den Offset (Position) des zu bearbeitenden Werts einzugeben
                cout << "Gib den Offset (dezimal) ein: ";
                cin >> offset;
                cin.ignore();  // Ignorieren des Zeilenumbruchs

                // Prüfen, ob der eingegebene Offset innerhalb des gültigen Bereichs liegt
                if (offset >= data.size()) {
                    cout << "Ungültiger Offset. Die Datei hat " << data.size() << " Bytes.\n";
                    break;  // Falls der Offset ungültig ist, wird die Bearbeitung abgebrochen
                }

                // Benutzer wählt das Eingabeformat für den neuen Wert
                cout << "Wähle die Eingabeform:\n";
                cout << "1. HEX\n";
                cout << "2. BIN\n";
                cout << "3. CHAR\n";
                cout << "Option (1-3): ";

                int format;
                cin >> format;
                cin.ignore();  // Ignorieren des Zeilenumbruchs

                // Speichert den aktuellen Wert an der gewählten Offset-Position
                unsigned char newValue = data[offset];

                if (format == 1) {  // Falls der Benutzer HEX-Eingabe gewählt hat
                    string hexStr;
                    cout << "Neuer Wert (2-stellige HEX, z.B. 1A): ";
                    cin >> hexStr;
                    cin.ignore();  // Ignorieren des Zeilenumbruchs

                    try {
                        // Konvertiert den HEX-String in eine Ganzzahl vom Typ 'unsigned char'
                        newValue = static_cast<unsigned char>(stoi(hexStr, nullptr, 16));
                    } catch (exception& e) {
                        cout << "Ungültige HEX Eingabe.\n";
                        break;  // Falls eine ungültige HEX-Eingabe gemacht wurde, Abbruch
                    }
                } else if (format == 2) {  // Falls der Benutzer BIN-Eingabe gewählt hat
                    string binStr;
                    cout << "Neuer Wert (8-stellige BIN, z.B. 00011010): ";
                    cin >> binStr;
                    cin.ignore();

                    // Konvertiert den Binär-String in ein Byte
                    if (!binaryStringToByte(binStr, newValue)) {
                        cout << "Ungültige BIN Eingabe.\n";
                        break;  // Falls eine ungültige BIN-Eingabe gemacht wurde, Abbruch
                    }
                } else if (format == 3) {  // Falls der Benutzer CHAR-Eingabe gewählt hat
                    char ch;
                    cout << "Neuer Wert (ein Zeichen): ";
                    cin >> ch;
                    cin.ignore();

                    // Speichert das eingegebene Zeichen als neuen Byte-Wert
                    newValue = static_cast<unsigned char>(ch);
                } else {
                    cout << "Ungültige Formatwahl.\n";
                    break;  // Falls eine ungültige Formatwahl gemacht wurde, Abbruch
                }

                // Speichert den neuen Wert an der gewählten Offset-Position
                data[offset] = newValue;
                cout << "Wert bei Offset " << offset << " aktualisiert.\n";
                break;  // Beendet den Fall 1 des Switch-Blocks
            }
            case 2: {  // Fall 2: Die Datei soll gespeichert und das Programm beendet werden
                // Öffnet die Datei im Binärmodus zum Schreiben
                ofstream outFile(filename, ios::binary);

                // Überprüft, ob die Datei erfolgreich geöffnet wurde
                if (!outFile) {
                    cerr << "Fehler beim Schreiben der Datei.\n";  // Fehlermeldung ausgeben
                    break;  // Falls das Öffnen der Datei fehlschlägt, wird die Speicherung abgebrochen 
                }

                // Schreibt den Inhalt des Vektors 'data' in die Datei
                outFile.write(reinterpret_cast<const char*>(data.data()), data.size());

                // Schließt die Datei nach dem Schreiben
                outFile.close();

                cout << "Datei gespeichert.\n";
                exitLoop = true;  // Beendet die Schleife und damit Programm
                break;  // Beendet den Fall 2 des Switch-Blocks
            }
            case 3: {  // Fall 3: Das Programm soll beendet werden, ohne die Datei zu speichern
                cout << "Beenden ohne zu speichern.\n";
                exitLoop = true;  // Beendet die Schleife und das Programm
                break;  // Beendet den Fall 3 des Switch-Blocks
            }
            default:  // Falls der Benutzer eine ungültige Option eingibt
                cout << "Ungültige Option. Bitte wähle 1, 2 oder 3.\n";
                break;  // Beendet den Default-Fall des Switch-Blocks
        }
    }  // Ende der while-Schleife

    return 0;  // Programm erfolgreich beendet
}
 