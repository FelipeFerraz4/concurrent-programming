#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_USUARIOS 10
#define TAM_EMAIL 100
#define TAM_SENHA 100

typedef struct {
    char email[TAM_EMAIL];
    char senha[TAM_SENHA];
} Usuario;

Usuario usuarios[MAX_USUARIOS];
int num_usuarios = 0;

void desativar_echo() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);  
    t.c_lflag &= ~ECHO;           
    tcsetattr(STDIN_FILENO, TCSANOW, &t);  
}

void ativar_echo() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);  
    t.c_lflag |= ECHO;            
    tcsetattr(STDIN_FILENO, TCSANOW, &t);  
}

void registrar_usuario(char *email, char *senha) {
    system("clear");
    printf("Cadastro de Usuário:\n");
    printf("Digite seu email: ");
    scanf("%s", email);  
    
    printf("Digite sua senha: ");
    desativar_echo();  
    scanf("%s", senha);
    ativar_echo();  
    printf("\n");

    while(strlen(senha)>3){
        printf("Por favor, digite uma senha com no máximo 4 números: ");
        desativar_echo();  
        scanf("%s", senha);
        ativar_echo();  
        printf("\n");
    };

    strcpy(usuarios[num_usuarios].email, email);
    strcpy(usuarios[num_usuarios].senha, senha);
    num_usuarios++;

    system("clear");
    printf("Usuário registrado com sucesso!\n\n");
}

int login_usuario(char *email_tentativa, char *senha_tentativa) {
    for (int i = 0; i < num_usuarios; i++) {
        if (strcmp(usuarios[i].email, email_tentativa) == 0 && strcmp(usuarios[i].senha, senha_tentativa) == 0) {
            return 1;  
        }
    }
    return 0;  
}   

void login_visualizar(){
    system("clear");
    
    char email_tentativa[100], senha_tentativa[100];

    printf("Login:\n");
    printf("Digite seu email: ");
    scanf("%s", email_tentativa);  

    
    printf("Digite sua senha: ");
    desativar_echo();  
    scanf("%s", senha_tentativa);  
    ativar_echo();  
    printf("\n");

    system("clear");
    if (login_usuario(email_tentativa, senha_tentativa)) {
        printf("Login bem-sucedido! Bem-vindo!\n\n");
    } else {
        printf("Credenciais incorretas. Tente novamente.\n\n");
    }
}

void login_bruto(){

}

void login(){
    system("clear"); 
    int opcao;

    printf("Escolha a opção de login:\n");
    printf("1 - Login padrão\n");
    printf("2 - Tentativa e erro\n");

    scanf("%d", &opcao);

    switch (opcao) {
        case 1:
            login_visualizar();
            break;
        case 2:
            login_bruto();
            break;
        default:
            printf("Opção inválida! Tente novamente.\n");
    }
}

void ver_usuarios() {
    if (num_usuarios == 0) {
        printf("Nenhum usuário registrado.\n\n");
        return;
    }

    printf("Usuários registrados:\n");
    for (int i = 0; i < num_usuarios; i++) {
        printf("Email: %s\n", usuarios[i].email);  // Exibe o email, mas não a senha
    }
    printf("\n\n");
}

int main() {
    char email_registrado[100], senha_registrada[100];   


    while(1){
        int opcao;
        
        printf("Bem-vindo ao sistema de registro e login!\n");
        printf("Escolha uma opção:\n");
        printf("1. Registrar\n");
        printf("2. Login\n");
        printf("3. Ver usuários registrados\n");
        printf("4. Sair\n");
        printf("Digite sua opção: ");

        scanf("%d", &opcao);

        switch (opcao) {
                case 1:
                    registrar_usuario(email_registrado, senha_registrada);
                    break;
                case 2:
                    login();
                    break;
                case 3:
                    // Ver usuários registrados
                    ver_usuarios();
                    break;
                case 4:
                    // Sair do programa
                    printf("Saindo...\n");
                    return 0;
                default:
                    printf("Opção inválida! Tente novamente.\n");
        }
    }

    return 0;
}
