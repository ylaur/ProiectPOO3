# Descrierea Proiectului: Mini Aplicatie Bancara (POO)
Acest proiect este o aplicație care simulează funcționarea unui sistem bancar. Obiectivul meu principal nu a fost doar să stochez niște date, ci să construiesc un sistem cu logică coerentă, care să valideze informații, să aplice reguli financiare și să prevină erorile sau chiar fraudele.

În continuare, voi detalia cum am gândit și implementat componentele principale ale aplicației:

# 1. Clasa Client și gestionarea memoriei
Clasa Client reține detaliile proprietarului unui cont. Aici am vrut să demonstrez că știu să lucrez cu memoria alocată dinamic, așa că am stocat numele folosind un pointer (char* nume). Pentru a preveni memory leaks, am implementat "Regula celor 3" (Rule of Three). Mi-am scris propriul destructor, un constructor de copiere și am supraîncărcat operatorul de atribuire folosind metoda Copy and Swap Idiom, care asigură o copiere sigură a datelor.
Cea mai interesantă metodă de aici este validareCNP. Nu verific doar dacă CNP-ul are 13 cifre, ci am scris un algoritm real care validează structura lui: verifică anul, luna, ziua, codul de județ și calculează Cifra de Control, ca să mă asigur că nu se pot crea clienți cu date false.

# 2. Clasa Tranzactie
Aceasta salvează detaliile unui transfer bancar. Chestiă interesanta la această clasă este că nu îi cer utilizatorului să introducă data și ora de mână. Am folosit biblioteca <ctime> ca să fac un apel direct la sistemul de operare (localtime). Când se creează o tranzacție, clasa face singură conversiile și își formatează data sub formă de text ("YYYY/MM/DD") și ora exactă, adăugând automat zerourile necesare pentru un format ușor de prelucrat.

# 3. Programarea Generică: Clasa template Registru<T>
Pentru a nu repeta codul, am creat un template numit Registru<T>. Acesta funcționează folosind containerul std::vector din C++. Avantajul este că am putut refolosi exact același cod în două locuri complet diferite: o dată la nivelul conturilor, sub forma Registru<Tranzactie>, pentru a ține istoricul fiecărui client, și a doua oară la nivelul băncii, ca Registru<std::string>, pentru a avea un jurnal de sistem (log-uri) general.

# 4. Ierarhia de Conturi și detectarea fraudelor
Cont este o clasă abstractă, baza întregului sistem. Două metode importante ies în evidență:

genereazaIban(): Când se deschide un cont, i se atribuie un IBAN unic. Am simulat generarea reală: generez un cod de bancă din litere aleatoare, 16 cifre aleatoare, le convertesc pe toate în format numeric și aplic algoritmul Modulo 97 (folosit internațional) pentru a calcula corect Cifra de Control specifică României ("RO").

verificaAlertaFrauda(): Am implementat un sistem de protecție a conturilor. La fiecare operațiune, metoda se uită în istoricul contului. Dacă vede că s-au făcut 5 tranzacții diferite către exact același IBAN destinatar, în decursul aceluiași minut, returnează valoarea "true", care înseamnă că acel cont trebuie blocat.

Din Cont am derivat două clase. ContEconomii (care nu permite retrageri dacă nu ai bani suficienți) și ContCredit (care îți permite să mergi pe minus până la o anumită limită). Ambele au o metodă suprascrisă estimeazaSoldViitor(int ani), care calculează matematic dobânda compusă (DC = (1 + r)^n, r = rata si n = numărul de ani).

# 5. Design Patterns
Pentru a face codul ușor de extins, am implementat 3 design patternuri:

Singleton: Clasa SistemBancar este centrul aplicației. Prin Singleton, mă asigur că programul nu poate crea accidental două bănci paralele. Totul se gestionează printr-o singură instanță centrală.

Strategy: Banca are mai multe tipuri de comisioane de retragere (Angajatul are 0.7%, Standard e 1%, Studentul are 0%). În loc să fac structuri lungi de if, respectiv else, pentru clasele de conturi am creat o clasa PercepeComision și clase derivate pentru fiecare calcul (atribuirea comisionului pentru fiecare client).

Factory: Clasa ContFactory este responsabilă cu fabricarea conturilor. Aceasta unește logica de creare și oprește procesul (aruncă o excepție) dacă încerci să creezi un cont folosind un client cu un CNP invalid.

# 6. Logica tranzacțiilor și Căutarea complexă în Sistemul Bancar
Clasa SistemBancar are două funcții extrem de interesante:

transferaFonduri: Când se mută bani, funcția scoate întâi suma din contul sursă și apoi încearcă să o pună în contul destinație. Dacă se întâmplă vreo eroare pe parcurs (prinsă cu un bloc try-catch), sistemul face "rollback", adică pune banii la loc în contul sursă și anulează tranzacția, notând eroarea în jurnal ca să nu se evapore banii pe drum.

filtreazaConturi<T, U>: Aceasta este o funcție template cu doi parametri, folosită pentru căutare internă (filtrarea conturilor care îndeplinesc o anumită condiție). În main(), mă pot folosi de ea împreună cu expresii lambda pentru a extrage exact ce am nevoie. De exemplu, îi trimit funcției o regulă (precum „returnează-mi doar conturile de Economii care au peste 5000 RON”), ea merge prin toate conturile, face dynamic_cast pentru tipul corect de conturi și îmi aduce rezultatele într-un mod sigur, fiind protejată la rândul ei de un try-catch "catch-all" (catch(...)).

### În concluzie, consider că proiectul îndeplinește toate cerințele. Am integrat următoarele: polimorfism, excepții propii, templateuri și design patterns, bifând astfel tot ce este necesar.
