#include <stdio.h>
#include <windows.h>
#include <time.h>
//#include <unistd.h>			// Koen vanwaar komt dit?

HANDLE initCom(int comPortNumber) {
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;
    char comPortName[20];
    sprintf(comPortName, "\\\\.\\COM%d", comPortNumber);
    hCom = CreateFileA(comPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, NULL);
    if (hCom == INVALID_HANDLE_VALUE) {
        MessageBoxA(0, "Fout: De seriele poort kan niet geopend worden!","Fout", MB_OK | MB_ICONEXCLAMATION);
        exit(-1);
    }
    fSuccess = GetCommState(hCom, &dcb);
    if (!fSuccess) {
        MessageBoxA(0, "Fout: De instellingen van de seriele poort kunnen niet gelezen worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
        exit(-1);
    }
    dcb.fParity = TRUE;
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 7;
    dcb.Parity = EVENPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    fSuccess = SetCommState(hCom, &dcb);
    if (!fSuccess) {
        MessageBoxA(0, "Fout: De instellingen van de seriele poort kunnen niet aangepast worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
        exit(-1);
    }
    return hCom;
}

char readCom(HANDLE hCom) {
    char inBuffer[2] = " ";
    unsigned long nBytesRead;
    while (ReadFile(hCom, &inBuffer, 1, &nBytesRead, NULL) && nBytesRead == 0);
    if (nBytesRead != 1) {
        MessageBoxA(0, "Fout: De seriele poort kan niet uitgelezen worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
        exit(-1);
    }
    return inBuffer[0];
}

void writeCom(HANDLE hCom, char c) {
    char outBuffer[2] = " ";
    unsigned long nBytesWritten;
    outBuffer[0] = c;
    WriteFile(hCom, &outBuffer, 1, &nBytesWritten, NULL);
    if (nBytesWritten != 1) {
        MessageBoxA(0, "Fout: De seriele poort kan niet beschreven worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
        exit(-1);
    }
}

void closeCom(HANDLE hCom) {
    CloseHandle(hCom);
}

int main(void) {
    HANDLE hCom;
    char in;
    FILE* outfile;
    time_t tijd;
    struct tm* pt;
    outfile = fopen("output.txt", "w");		// Koen: Maak tekstbestand aan
    if (outfile == NULL) {
        printf("File output.txt kan niet aangemaakt worden.\n");
        return 1;
    }
    hCom = initCom(1);
    
    while(1){
        printf("Er wordt nu een S verstuurd.\n");
        writeCom(hCom, 'S');
        printf("Ontvangen karakters worden op het scherm gezet totdat een Q wordt ontvangen.\n");
	/* De tijd en datum ophalen uit het operating systeem */
        time(&tijd);
        pt = localtime(&tijd);
        fprintf(outfile,"%02d-%02d-%4d %02d:%02d:%02d :", pt->tm_mday, pt->tm_mon + 1, pt->tm_year + 1900, pt->tm_hour, pt->tm_min, pt->tm_sec);
        do {
        in = readCom(hCom);
            if (in != 'Q') {
                putchar(in);
	        fprintf(outfile,"%c",in);		// Koen: Print character naar file
            }
        } while (in != 'Q');
	fprintf(outfile,"\n");
	Sleep(5000);
	printf("\n");							// Beetje netter uitzien
    }
    closeCom(hCom);
    fclose(outfile);
    printf("File output.txt is aangemaakt.\n");
    printf("\nEINDE.\n");
    getchar();
    return 0;
}
