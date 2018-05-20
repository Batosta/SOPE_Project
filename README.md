TODO:

CLIENT

1-Recebe como argumentos da consola																(DONE)
(client <time_out> <num_wanted_seats> <pref_seat_list>).

2-cria FIFO ansXXXX.																		(DONE)

3-Envia ao servidor através do requests um pedido, pode ser através de uma struct.										(DONE)

4-Aguarda a resposta.

5-Implementar o TIME_OUT.

6-Escrever no clog.txt.

Bem sucedido(PID XX.NN IDENTIFICADOR)

Mal sucedido(PID ERRO)

	XX.NN - 01.03 ou seja lugar um de tres reservados.

7-Escrever no cbook.txt com WITH_SEAT (IDENTIFICADOR) isto é cada cliente neste apenas escreve cada lugar reservado por cada linha.

Não é preciso PID neste.





SERVER

1-Recebe como argumentos da consola																(DONE)

(server <num_room_seats> <num_ticket_offices> <open_time>).

2-Cria o FIFO requests.																		(DONE)

3-Cria o numero de threads por bilheteira.															(DONE)

4-O programa principal lê dos requests.(falta guardar no buffer)

5-Apenas lê quando o buffer está vazio.

6-As bilheteiras apanham o buffer.

7-Mutex para ler do buffer para só estar uma de cada vez a do mesmo.

8-Reserva dos lugares.

9-Casos de erro:

-1 a quantidade de lugares pretendidos é superior ao máximo permitido (MAX_CLI_SEATS).

-2 o numero de identificadores dos lugares não é válido.

-3 os identificadores dos lugares pretendidos não são válidos.

-4 outros erros nos parametros.

-5 pelo menos um dos lugares pretendidos não está disponível.

-6 sala cheia.

10-Array de mutex's uma para cada lugar ,

para impedir que haja 2 bilheteiras ou mais a tentarem reservar o mesmo lugar ao mesmo tempo.

Funções que as threads devem ter:(VER SE ESTA BEM)
  
	int isSeatFree(Seat * seats,int seatNum)- testa se o lugar seatNum está livre.

	void bookSeat(Seat * seats, int seatNum, int clientId)-reserva o lugar seatNum.
	void freeSeat(Seat * seats, int seatNum)- liberta o lugar seatNum, para os casos
		em que já foi reservado mas não foram lugares suficientes.
11-Aplicar um #define DELAY() nestas funções para simular a demora na execução.

12-Enviar resposta ao cliente (criar uma struct asnwer?).

13-Aplicar o time_out.

14-Quando for para dar time_out a main manda as bilheteiras terminarem e aguarda que elas terminem.

15-Cada thread regista no slog.txt a abertura e fecho de si mesmo.

16-Cada bilheteira escrever no slog.txt o registo dos lugares reservados.

(TO-CLIID-NT: aaaa bbbb cccc dddd... -XXX)

	TO - Id da bilheteira.

	CLIID - PID cliente.

	NT - numero de lugares reservados.

	aaaa/bbbb/cccc - lugares preferidos.

		XXX - lugares reservados/ERRO.

	Para cada caso de erro:

		-1 MAX.

		-2 NST.

		-3 IID.

		-4 ERR.

		-5 NAV.

		-6 FUL.
17-Escrever no sbook.txt o identificador do lugar reservado, um por linha, com WIDTH_SEAT=4.
