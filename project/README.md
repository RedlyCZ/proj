# Financial Analyzer

**Financial Analyzer** je kvantitativní nástroj pro správu a analýzu finančního portfolia ovládaný přes příkazovou řádku. Aplikace umožňuje sledovat akcie, hotovost a kryptoměny, archivovat jejich stav v čase a provádět výpočty budoucího vývoje či technickou analýzu.

## Hlavní funkce
* **Správa aktiv:** Evidence akcií, měnových pozic a kryptoměn v reálném čase.
* **Kvantitativní analýza:** Výpočet RSI, Bollingerových pásem a pravděpodobnostní simulace Monte Carlo.
* **Backtesting a predikce:** Porovnání s historickými daty a odhad budoucích výnosů pomocí složeného úročení.
* **Perzistence:** Ukládání a načítání stavu portfolia pomocí JSON snapshotů.

## Požadavky
Pro úspěšné sestavení a běh programu jsou vyžadovány následující prerekvizity:
* **Standard C++:** Kompilátor s podporou C++23.
* **Sestavovací systém:** CMake verze 3.15 nebo vyšší.
* **Knihovny:** Projekt využívá `cpr` pro síťovou komunikaci a `nlohmann/json` pro zpracování dat (obě se stahují automaticky během procesu sestavení).
* **Připojení k internetu:** Nutné pro stahování dat z finančních API.

## Sestavení a instalace
V kořenovém adresáři projektu spusťte následující příkazy:

```
mkdir build
cd build
cmake ../src
cmake --build .
```

## Konfigurace (API klíče)
Aplikace využívá externí finanční služby, které vyžadují autentizaci pomocí API klíčů. Před prvním spuštěním vytvořte v adresáři s binárkou soubor `config.json` s následující strukturou:

```
{
    "finnhubApiKey": "VAS_KLIC",
    "twelveDataApiKey": "VAS_KLIC",
    "apiNinjaApiKey": "VAS_KLIC",
    "fredApiKey": "VAS_KLIC"
}
```
*Poznámka: Bezplatné klíče lze získat po registraci u poskytovatelů FinnHub, TwelveData, API Ninjas a FRED.*

## Spuštění
Program spustíte příkazem:
```bash
./financialAnalyzer
```
Po spuštění se zobrazí interaktivní CLI rozhraní připravené k přijímání vašich příkazů.

## Adresářová struktura
* `src/` – Zdrojové soubory aplikace.
* `docs/` – Uživatelská a programátorská dokumentace.
* `config.json` – Konfigurační soubor s API klíči (vytváří uživatel).
