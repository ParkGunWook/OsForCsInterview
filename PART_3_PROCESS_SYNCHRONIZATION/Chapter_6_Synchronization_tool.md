# what we gonna study

**Cooperating process**는 시스템에서 존재하는 다른 프로세스에게 영향을 주거나 받는 것이다. 협업 프로세스들은 논리적인 주소공간을 직접 공유하거나 공유 메모리 또는 메시지 패싱만으로 데이터를 공유할 수도 있다. 공통 데이터에 동시 접근은 데이터 불완전성을 초래할 수 있다. 이 장에서는, 우리는 협업 프로세스들이 순서대로 작동하게 하는 다양한 메커니즘을 배워서, 데이터 일관성을 유지시키겠다.

# Objectives

- 임계-영역 문제와 레이스 컨디션에 대해서 설명한다.
- 하드웨어 솔루션인 메모리 베리어, 컴패어-스왑 명령어, atomic 변수로 임계영역 문제 해결법 설명하겠다.
- 뮤텍스 락, 세마포, 모니터, condition variables로 임계영역 문제를 해결하겠다.
- low, moderate, high 경쟁 상황에 따른 임계영역 문제 해결 툴을 측정하겠다.

## 6.1 Background

우리는 concurrently or parallel하게 작동하는 프로세스를 보았다. 3.3.2 절에서는 프로세스 스케쥴링의 역할을 소개하고 어떻게 CPU 스케쥴러가 빠르게 스위칭하면서 concurrent하게 작동하는지 보여줬다. 실제로, 이것은 한개의 프로세스가 다른 프로세스가 스케쥴되기전에 부분적으로 완료할 수 있다는 것이다. 실제로, 프로세스는 특정 명령어의 스트림에서 인터럽트되고, 프로세싱 코어는 다른 프로세스의 명령어를 할당한다. 4.2절에서는 병렬 실행, 두개의 명령어 스트림이 구별된 코어에서 나오는 것을 보였다. 이 챕터에서는, 우리는 어떻게 병렬 실행이 몇가지 프로세스가 공유하는 데이터의 무결성에 이슈를 만드는지 설명하겠다.

어떻게 이런 일이 생기는지 알아보자. 3장에서, 우리는 연속 협업 프로세스 또는 스레드의 모델을 보았고, 비동기로 작동하고 데이터를 공유했다. 우리는 생산자-소비자 문제로 모델을 설명했고, 운영체제 함수의 대표 패러다임이다. 특별하게도, 3.5절에서, 우리는 메모리를 공유하기 위해서 프로세스에 바운디드 버퍼를 주는 것을 설명했다.

우리는 이제 바운디드 버퍼의 고려로 돌아오겠다. 말했듯이, 우리의 본래의 해결책은 BUFFER_SIZE-1의 아이템이 버퍼에 있을떄였다. 우리가 이 알고리즘의 결점을 감소하려고 수정해보겠다. 한가지 가능성은 정수 변수, 카운트를 0으로 초기화하는 것이다. 카운트는 새로운 아이템이 버퍼에 들어올때마다 증가하고 빠질때마다 감소한다. 

생산자 코드는 다음과 같다.

```c
while(true){
    /* produce an item in next produced */
    
    while(count == BUFFER_SIZE) ;
    
    buffer[in] = next_produced;
    in = (in +1) % BUFFER_SIZE;
    count++;
}
```

소비자 코드는 다음과 같다.

```c
while(true) {
    while(count == 0) ;

    next_consumed = buffer[out];
    out = (out +1) % BUFFER_SIZE;
    count--;
}
```

비록 생산자와 소비자 루틴이 개별적으로는 옳아보이지만, 그들은 동시에 작동할때 문제가 생길 수 있다. count의 값이 현재 5이고 생산자와 소비자가 동시에 "count++", "count--"를 실행한다고 생각해보자. 두개의 실행은, 4 또는 5 또는 6이 될 수 있다. 기계어로 count++이 해석될때에 총 3가지 단계를 다음과 같이 가진다. 그리고 이 기계어가 순서에 맞지 않게 끼워지면 문제가 생기는 것이다.

> register_1 = count
> register_1 = register_1 + 1
> count = register_1

우리는 우리가 두개의 프로세스가 count 변수를 동시에 조작해서 문제가 된것을 알고있다. 몇가지 프로세스가 같은 데이터를 동시에 접근하고 조작하는 상황에서는 실행의 결과는 접근이 일어난 시점의 특정한 순서에 따라서 달라지고 **race condition**이라고 부른다. 이런 race condition을 막기위해서는, 우리는 count를 동시에 한개의 프로세스만 조작하게하면된다. 이것을 보장하기 위해서, 우리는 프로세스가 몇가지 방식으로 동기화 되게 한다.

운영체제에서는 시스템의 다른 파트가 자원을 관리할때 자주 일어난다. 더욱이, 우리가 앞서 언급했듯이, 멀티코어 시스템의 발전은 멀티스레드 앱의 개발의 중요성을 올렸다. 이런 앱에서, 데이터를 공유하는 몇가지 스레드들은 다른 프로세싱 코어에서 병렬로 작동중이다. 확실하게, 우리는 이런 활동들이 다른 것을 방해하는 변화를 원하지 않는다. 이 이슈의 중요성 때문에, 우리는 **process synchronization**과 **coordination**에 많은 투자를 했다.


## 6.2 Critical-Section Problem

우리는 프로세스 동기화의 첫 번째 고려를 임계-영역 문제로 시작하겠다. n개의 프로세스를 가진 시스템을 생각하겠다. 각각의 프로세스는 **critical-section**이라는 공유된 데이터가 하나 이상의 프로세스에게 접근되고 업데이트되는 코드의 부분이 있다. 시스템의 중요한 기능은,한 프로세스가 임계영역을 실행중이면, 다른 프로세스는 임계영역을 실행하는게 허용되지 않는다. 각 프로세스는 그것의 임계영역에 들아가기전에 승인 요청을 해야한다. 이런 요청을 하는 코드를 **entry section**이라고 한다. 그리고 임계영역 후에는 **exit section**을 가진다. 나머지 코드는 **remainder section**이라고 부른다. 

3가지 조건으로 임계영역 문제를 해결할 수 있다.

1. Mutex exclusion - 만약 P1이 임계영역을 실행중이면, 다른 프로세스들은 그들의 임계영역을 실행할 수 없다.
2. Progress - 만약 프로세스가 임계영역을 실행중이지 않고 몇몇 프로세스가 임계영역 실행을 원하면, 그들의 잔여 섹션를 실행중이지 않은 코드들중 하나가 선택되어서 임계영역을 실행한다. 그리고 이 선택은 무기한으로 연기될 수 없다.
3. Bounded waiting - 만약에 바운드, 리미트가 시간에 따라 존재하면, 다른 프로세스들은 프로세스가 임계영역에 입장 요청을 보낸후에 그들의 임계 영역에 들어가는 것을 허용받는다.

프로세스들은 0이 아닌 속도로 실행중이라고 가정한다. 그러나 우리는 n프로세스의 상대적인 속도를 추측할 수 없다.

주어진 시점에서, 많은 커널 모드 프로세스는 운영체제 안에서 활동중이다. 결과적으로, 운영체제를 구현하는 코드(커널 코드)는 몇가지 레이스 컨디션을 만들수 밖에 없다. 커널 데이터 구조가 시스템에서 오픈 파일의 리스트를 유지한다. 이 리스트는 반드시 새로운 파일이 열리고 닫히면 수정되어야한다. 만약 두가지 프로세스들이 파일을 열려고하면, 리스트를 향한 분리된 업데이트는 레이스 컨디션을 유발한다. 

다른 예시는 두가지 프로세스 P0와 P1이 fork() 시스템 콜로 자식 프로세스를 만든다고 생각하자. 이 예제에서, 다음의 프로세스 식별자의 값인 커널 변수 next_available_pid에서 레이스 컨디션은 생길수 있다. 상호 배제가 없다면, 같은 프로세스 식별자가 두개의 분리된 프로세스에 배정된다.

레이스 컨디션을 잘 일으키는 다른 커널 데이터 구조는 메모리 할당을 위한 구조, 프로세스 리스트 관리, 인터럽트 핸들링을 포함한다. 이것은 커널 개발자에게 운영체제가 레이스 컨디션에 자유롭도록할지 달려있다.

임계 영역 문제는 싱글 코어 환경에서는 만약 공유 변수가 실행중일때 인터럽트를 방지함으로서 간단하게 해결할수있다. 이런 방법으로, 우리는 현재 명령어의 시퀀스가 선점없이 명령의 순서를 지키게 할수 있다. 다른 명령어들은 실행되지 않고, 예측하지 못한 변경은 생기지 않을 것이다.

불행히도, 이 해결책은 멀티프로세서 환경에서는 적절하지 않다. 멀티 프로세서에 대해서 인터럽트를 비활성화하는 것은 모든 프로세스끼리 메시지를 주고 받아야해서 비용이 많이 든다. 이 메시지 패싱은 임계영역에 들어갈때마다 딜레이시키고, 시스템 효율성은 감소한다. 또한 시스템의 클락이 인터럽트에 의해서 업데이된다면 그 효과를 생각해보아라.

두가지 방법이 운영체제에서 임계영역을 다루기 위해서 사용된다. **Preemptive kernels**과 **Nonpreemptive kernels**이다. 선점 커널은 커널 모드 실행중에 선점이 가능하게 허용하고 비선점은 커널에서 실행중인 프로세스가 선점되지 않게 한다. 커널 모드 프로세스는 커널 모드를 종료, 블락, 자발적으로 CPU의 컨트롤을 포기할때까지 실행된다.

명백하게, 비선점 커널은 커널 데이터 구조에서 레이스 컨디션으로 부터 자유로운데, 한개의 프로세스가 커널에서 한개만 실행되기 때문이다. 우리는 선점 커널에서는 같게 얘기하지 못하는데, 그들은 공유 커널 데이터를 레이스 컨디션으로부터 자유롭도록 보장해야한다. 선점 커널들은 SMP 디자인에서 특히 디자인하기 힘든데, 두개의 커널 모드 프로세스가 CPU 코어에서 동시에 실행할수있기 때문이다.

왜, 우리는 선점을 비선점보다 선호할까? 선점 커널은 더욱 반응성있고, 커널 프로세스가 너무 길게 작동하기전에 기다리는 프로세스를 위해서 포기하기 때문이다. 더욱이, 선점 커널은 리얼타임 프로그래밍에 적합하고, 리얼 타임 프로세스는 커널에서의 프로세스를 선점한다.

## 6.3 피터슨 솔루션

다음으로, 우리는 피터슨 솔루션이라는 임계영역 문제를 해결하는 소프트웨어 기반 해결법을 보겠다. 현대 컴퓨터 구조가 *load*나 *Store*같은 기본 기계어 명령어로 작동하기에, 우리는 피터슨의 해결책이 이런 구조에서 잘 작동한다는 보장이 없다. 그러나, 우리는 이 해결법이 임계영역을 해결하는 좋은 알고리즘 설명이 있고 소프트웨어를 디자이할 때 상호 배제, 프로그레서, 바운디드 웨이팅의 필요를 알려주는 복잡성을 보이기 때문에 소개하겠다. 

피터슨 해결법은 임계영역과 나머지 영역을 번갈아가며 실행하는 두개의 프로세스로 제한된다. 프로세스의 이름은 j = 1-i인 P_i와 P_j로 대표된다.

피터슨 방법은 두개의 프로세스가 turn과 flag[2]라는 2개의 데이터 아이템을 공유하도록 한다. 
```c
while(true) {
    flag[i] = true;
    turn = j;
    while(flag[j] && turn == j) ;
        
        /* critical section */
    
    flag[i] = false;

        /* remainder section */
}
```
여기서 변수 turn은 임계영역에 들어가야하는 프로세스의 차례를 알린다. 즉, 만약 turn == i이면, P_i가 임계영역에서 실행되도록 허가된다. flag 행렬은 만약 프로세스가 그것의 임계영역으로 들어갈 준비가 되었는지를 알리기 위해서 사용된다. 예를 들어서, 만약 flag[i]가 참이면 P_i가 임계영역에 들어가 준비가 되었다. 

P_i가 임계영역에 들어갈려면 flag[i]는 참이고 turn은 j여야하고, 만약 다른 프로세스가 임계영역에 들어가기를 주장하면, 그것은 가능하다. 만약 두개의 프로세스가 같은 시간에 들어가려하면, turn은 꽤나 같은 시간에 i와 j로 세팅될수 있다. 오직 하나의 할당만 남는다. 다른 것은 즉시 덮어쓰일 것이다. turn의 값은 마침내 두 프로세스중 어떤 것이 먼저 임계영역에 들어갈지 허가할 것이다.

우리는 이 해결법이 옳다고 증명하겠다.

1. 상호배제는 보장된다.
2. Progress 필요는 만족된다.
3. 바운디드 대기또한 만족된다.

1번째 영역은, 우리가 P_i가 오직 flag[j] == false or turn == i일때 접근한다는 것을 알고있다. 또한, 만약 두개의 프로세스가 임계영역 실행중이면, flag[0]==flag[1]==true일 것이다. 이 두개의 관측은 P0와 P1이 그들의 while 문을 정상적으로 작동하지 못한다는 것인데, turn의 값은 0또는 1일수 밖에 없기 때문이다. 따라서, 한개의 프로세스는 반드시 while문을 실행중이어야하는 것이다. 반면에 P_i는 적어도 하나의 추가적인 명령어를 ("turn==j") 실행해야한다. 그러나, flag[j] == true이고 trun ==j이면, P_j는 임계영역에 있을 것이다. 결과적으로 상호배제를 시킨 것이다.

두번째 영역과 세번째 영역을 증명하려면, P_i는 만약 flag[j] == true and turn==j의 while 상태에 빠져서 임계영역에 못들어간다는 것을 명심해야한다. 루프는 오직 한가지 가능성만 있다. 만약 P_j가 준비되지 않으면, flag[j] == false일 것이고, P_i는 임계영역에 들어간다. 만약 P_j가 flag[j]를 참으로하고 while문을 실행중이라면, turn==i or turn==j일 것이다. 만약에 turn == i이면, P_i가 임계영역에 들어간다. 만약에 turn == j이면 P_j가 실행된다. 그러나, P_j가 임계영역을 탈출하면, flag[j]는 거짓으로 리셋되고 또한 P_i가 임계영역으로 진입하게 한다. 만약 P_j가 flag[j]를 참으로 설정하면, 그것은 또한 turn을 i에게 줄것이다. 그러므로, P_i는 아무런 값을 바꾸지 않아도 P_i는 임계영역으로 진입하게 되고(Progress) P_j에 의해서 진입을 한다.(bounded waiting)

앞의 절에서 말했듯이, 피터슨 해결법은 현대 컴퓨터 구조에서는 작동이 보장되지 않는다. 시스템 성능을 향상시키기 위해서, 프로세서/컴파일러들은 의존성없는 read와 write를 재배치하기 때문이다. 싱글 스레드 앱을 위해서, 이 재배치는 프로그램이 최종값이 예상한것과 일치하게 할당되는 정확한 동작을 하기에 중요하지 않다.(이것은 수표장부를 밸런싱하는것과 비슷하다. 인출과 입금의 순서는 중요하지 않고 마지막 잔액이 중요한 것이다.) 그러나, 공유 데이터를 사용하는 멀티 스레드 앱에서는, 명령어의 재배치는 불일치와 예상하지 못한 결과를 초래할 수 잇다.

예를 들어서, 두개의 스레드 간에 공유되는 데이터로 아래와 같이 초기화된다.
`boolean flag = false;  int x = 0;`
스레드 1은 다음 문장을 실행한다.
`while(!flag);    print x;`
그리고 스레드 2는 다음을 실행한다.
`x = 100;    flag = true;`
예상되는 행동은 물론, 스레드 1에서 변수 x는 출력 결과가 100이 되는 것이다. 그러나 flag와 x사이에 아무런 의존성이 없기 때문에, 프로세서는 스레드 2의 명령어를 재배치할 수 있다. 그래서 flag가 참으로 배치되고 x가 100으로 할당된다. 이런 상황에서, 스레드 1은 0을 결과로 내게된다. 덜 중요한 것은 프로세서는 스레드 1을 다시 재배치해서 flag를 불러오기전에 x의 값을 불러올 수 있는 것이다. 이게 일어나면 x가 스레드2에서 재배치되지 않아도 0을 출력하게 된다.

이게 어떻게 피터슨 해결법에 영향을 줄까? 만약 피터슨 해결법의 entry section의 두개의 명령어가 재배치되면 무슨일이 일어날까? 그러면 두개의 스레드가 동시에 임계영역에서 활동할 수 있다.

다음 절에서 우리는, 적절한 동기화 도구를 이용해서 상호배제를 만족시키겠다. 이런 툴은 원시적인 하드웨어 지원과 커널 개발자와 앱 프로그래머 모두가 쓸수있는 추상화, 하이 레벨, 스프트웨어 기반 API를 통해서 만들어낸다.

## 6.2 동기화를 위한 하드웨어 지원

우리는 임계 영역 문제 해결을 위한 소프트웨어 기반을 설명했다.(*소프트웨어 기반*이라는 이유는 알고리즘이 운영체제 또는 특정 하드웨어 명령어을 포함하지 않기 때문이다.) 그러나, 말했듯이, 소프트웨어 기반 해결법은 현대 컴퓨터 구조에서 제대로 작동한다는 보장이 없다. 이 절에서는, 우리는 임계영역 문제 해결을 위한 3가지 하드웨어 명령어를 소개하겠다. 원시적인 명령어는 직접 동기화 툴로 사용되거나, 그들은 추상적인 동기화 메커니즘의 기초로서 사용될 수 있다.

### 6.4.1 메모리 배리어

6.3절에서는, 우리는 시스템이 명령어를 재배치할 수 있고, 이 정책은 신뢰할수 없는 데이터로 이끈다. 컴퓨터 구조가 어떤 메모리가 보장될지 결정하는 기능을 가진 프로그램을 **memory model**이라고 한다. 일반적으로, 메모리 모델은 2가지 카테고리로 나뉜다.

1. Strongly ordered - 프로세서에서의 메모리 수정은 다른 모든 프로세서에 즉시반영이 된다.
2. Weakly ordered - 프로세서의 메모리 변경이 다른 프로세서에 즉시 보이지 않는다.

메모리 모델은 프로세서 타입에 따라 다르고, 커널 개발자들은 공유 메모리 멀티 프로세서에서의 메모리 수정의 가시성에 따라서 가정을 할 수 없다. 이 문제를 해결하기 위해서, 컴퓨터 구조는 어떤 메모리에서의 변경이든 모든 프로세서에 알리는 명령어를 제공함으로서, 메모리 수정이 다른 프로세서에서 작동하는 모든 스레드가 반영하게 해준다. 이런 명령어는 **memory barriers** 또는 **memory fences**라고 불린다. 메모리 베리어 명령어가 실행되면, 시스템은 모든 로드와 스토어가 뒤따라오는 로드와 스토어 이전에 완료되는 것을 보장한다. 그러므로, 만약 명령어가 재배치 되어도, 메모리 배리어가 스토어 명령어가 다른 프로세서에게 읽히기 이전에 완료되는 것을 보장한다. 

이제 명령어의 재배치가 잘못된 아웃풋을 보이는 예제를 메모리 배리어로 해결해보겠다.

만약 우리가 스레드 1에 메모리 배리어를 추가한다면
`while(flag) memory_barrier();  print x;`
우리는 플래그의 값이 x의 값이 로드 되기 이전에 로드되는 것을 보장할 수 있다.

비슷하게 만약 우리가 스레드 2의 할당사이에 메모리 배리어를 두겠다.
`x = 100;  memory_barrier();   flag = true;`
우리는 x의 할당이 flag이전에 할당되는 것을 보장하게 된다.

이런 관점에서 피터슨 해결법을 쓴다면, 우리는 entry 섹션에서의 두개의 할당사이에 메모리 베리어를 두어서 명령어의 재배치를 피할 수 있다. 메모리 배리어는 low-level 명령어이고 오직 커널 개발자만이 상호배제를 위해서 쓸 수 있다.

### 6.4.2 하드웨어 명령어

많은 현대 컴퓨터 시스템은 워드의 내용물을 테스트하고 수정하거나 두 워드의 내용물을 **atomically** 바꾸는 특별한 하드웨어 명령어를 제공한다. 우리는 이런 특별한 명령어를 임계영역 문제를 간단하게 해결할때 많이 쓰인다. 특정한 머신을 위해서 특정한 명령어를 사용하는 것보다는, 우리는 test_and_set()과 comapre_and_swap() 명령어로 이런 종류의 명령어를 추상화하는 주요한 개념을 추상화했다.

```cpp
bool test_and_set(bool *target){
    bool rv = *target;
    *target = true;

    return rv;
}
```

```cpp
do{
    while(test_and_set(&lock)) ;

    /* critical section */

    lock = false;
}while(true);
``` 

test_and_set() 명령의 가장 중요한 요소는 그것이 atomically하게 실행되는 것이다. 그러므로, 만약에 두개의 test_and_set() 명령어가 동시에 작동한다면, 그들은 몇가지 임의의 순서로 연속적으로 실행될 것이다. 만약에 기계가 test_and_set() 명령어를 지원한다면, 우리는 상호배제를 불 변수 lock를 거짓으로 선언함으서 구현할 수 있다.

```cpp
int compare_and_swap(int *value, int expected, int new_value){
    int temp = *value;

    if(*value == expected) 
        *value = new_value;
    
    return temp;
}
```

```cpp
while(true){
    while(compare_and_swap(&lock, 0, 1) != 0) ;

    /*critical section */

    lock = 0;
    
    /*remainder section */
}
```

compare_and_swap() 명령어는 test_and_set()명령어처럼, 두개의 워드에서 atomically 수행되지만, 두개의 워드의 내용을 바꾼다는 것에서 다른 메커니즘이다. 

CAS 명령어는 3가지 피연산자를 사용한다. value는 오직 (*value == expected) 일때만 new_value로 설정된다. 무관하게, CAS는 항상 value의 원래 값만 리턴한다. 이 명령어의 중요한 특성은 그것이 atomically 실행된다는 것이다. 그러므로, 만약 두개의 CAS가 동시에 작동하면, 그들은 임의의 순서로 연속적으로 실행된다.

CAS를 이용한 상호배제는 다음과 같다. 전역 변수(lock)는 0으로 초기화된다. compare_and_wap()을 실행하는 첫번째 프로세스는 lock을 1로 설정한다. 그것은 그것의 임계 영역에 들어가고, 기본의 lock은 여전히 예측한 값 0과 같기 때문이다. 프로세스가 그것의 임계영역을 탈출하면, lock는 다시 0이되고, 다른 프로세스가 그것의 임계영역에 들어가게 허용한다. 

```cpp
while(true) {
    waitin[i] = true;
    key = 1;
    while(waiting[i] && key == 1)
        key = compare_and_swap(&lock, 0, 1);
    waiting[i] = false;

    /* critical section */

    j = (i+1) % n;
    while( (j != i) && !waiting[j] )
        j = (j+1) % n;

    if(j==i)
        lock = 0;
    else
        waiting[j] = false;
    
    /*remainder section */
}
```
비록 이 알고리즘이 상호배제를 만족하지만, 그것은 바운디드 웨이팅은 만족하지 못한다. 우리는 모든 임계 영역 조건을 만족하는 것을 만들었다. 여기서 공통 데이터 구조는 다음과 같다.
`bool waiting[n];   int lock;`
waiting 행렬의 원소는 false로 초기화 되고, lock는 0으로 초기화 된다. 상호 배제가 충족되는 것을 입증하기 위해서, 우리는 P_i가 임계영역에 들어갈 때는 오직 waiting[i] == false or key == 0일 때이다. compare_and_swap()은 실행되면 key의 값은 0이 된다. 이를 통해서 다른 것들은 key가 0이므로 계속 대기할 것이다. 만약 waiting[i]가 거짓이 되는 것은 오직 다른 프로세스가 임계영역을 벗어났을 때이다. 따라서 상호-배제가 만족된다.

프로그레스 조건이 만족됨을 보이기 위해서, 우리는 상호배제에서 쓴 방법을 다시 생각해서, 임계영역을 탈출할때 lock이 0이되거나 waiting[j]가 거짓으로 될 것이다. 두가지는 모두 다른 대기중인 프로세스가 다음으로 임계영역에 들어가는 것을 허용한다.

바운디드 웨이팅을 만족하기 위해서는, 우리는, 프로세스가 임계영역을 떠날때, 그것은 waiting 행렬을 스캔하는 것을 고려할 수 있다. 그것은 이 순서의 첫번쨰 프로세스가 entry 섹션에 있으면 다음 것을 바로 임계영역에 들어가게 해준다. 어떤 대기중인 프로세스도 n-1안에 그 차례에 돌아가게 할 수 있다.

test_and_set()과 compare_and_swap()은 컴퓨터 구조에 관한 책에서 더욱 깊게 소개될 것이다.

### 6.4.3 Atomic Variables

일반적으로, CAS 명령어는 상호배제를 위해서 잘 쓰이지 않는다. 오히려 그것은 임계영역을 해결하는 다른 툴을 위한 기본적인 블락으로 쓰인다. 그러한 한가지 툴이 **atomic variable**이다. 이것은 인티저나 불값에 대해서 기본적인 atomic operation을 제공한다. 우리는 6.1에서 상수값을 증가/감소하는 것이 레이스 컨디션을 유발할 수 있다고 했다. 아토믹 변수는 한가지 데이터가 업데이트되면서 레이스 상황에 처할 때 상호배제를 확실하게 해줄때 쓰인다.

대부분의 시스템은 아토믹 변수를 특별한 아토믹 데이터 타입(아토믹 변수에 접근하고 조작하는 함수를 포함)으로 제공한다. 이런 함수들은 보통 CAS 명령어로 구현된다. 예시에서는 아토믹 정수 sequence가 증가되는 것을 보이겠다.
`increment(&sequence);`
여기서 increment함수는 CAS를 통해서 구현된다.
```c
void increment(atomic_int *v)
{
    int temp;

    do{
        temp = *v;
    }\
    while( temp != compare_and_swap(v, temp, temp+1) );
}
```

아토믹 변수는 아토믹 업데이트를 제공한다. 그들은 모든 상황에서 레이스 컨디션에 빠지지 않는다. 예를 들어서, 바운디드 버퍼 문제에서, 우리는 아토믹 정수를 count에 쓸 수 있다. 이렇게 해서 count는 atomic하게 업데이트된다. 그러나 생산자-소비자 프로세스는 또한 count의 값에 따라서 while 루프에 빠진다. 버퍼가 현재 비었고 두명의 소비자가 count>0이길 기다린다고 가정한다. 만약 생산자가 버퍼에 하나의 아이템을 넣으면 두 소비자는 while루프를 소비하려고 나온다. 물론 count는 1일 뿐이다.

아토믹 변수는 비록 그것이 카운터나 연속 생산자같은 공유데이터에만 쓰일수 있어도 운영체제에서 많이 쓰인다. 다음 절에서는, 더 좋은 툴을 보겠다.

## 6.5 Mutex Locks

6.4절에서 하드웨어 기반의 임계 영역 문제는 앱 프로그래머가 접근하기 힘들다. 대신에, 운영체제 디자이너는 하이레벨 소프트웨어 툴을 임계영역 문제 해결을 위해서 제공한다. 가장 간단한 도구는 **Mutex lock**이라고 한다. 우리는 뮤텍스 락을 임계영역 보호와 레이스 컨디션 방지를 위해서 사용한다. 즉, 프로세스는 반드시 임계영역에 도달하기 전에 락을 얻어야한다. 그리고 임계영역 종료휴에 락을 해제한다. acquire()함수는 락을 얻고, realease() 함수는 락을 해제한다.

뮤텍스 락은 *available*이라는 락이 있는지 없는지 알리는 불 변수를 가진다. 만약 락이 가용하면, acquire()은 성곤하고 락은 불가용해진다. 불가용한 락을 요청하는 프로세스는 락이 해제될때까지 블락된다.

acquire()의 정의는 다음과 같다.
```cpp
acquire(){
    while(!available) ; /* busy wait */
    available = false;
}
```
release()는 다음과 같다.
```cpp
release(){
    available = true;
}
```

acquire()와 realease()는 반드시 아토믹하게 실행되어야한다. 그러므로, 뮤텍스 락은 CAS 명령어를 이용해서 구현되고 우리는 이 기술의 설명을 예제로 두었다.

여기서 구현된 것의 가장 큰 문제는 **busy waiting**을 필요로 하는 것이다. 프로세스가 그것의 임계영역에 있는 동안, 그것의 임계영역에 들어가려는 프로세스는 반드시 acquire() 콜안의 루프에 갇혀야한다. 이 반복되는 루프는 한개의 CPU 코어가 많은 프로세스들에게 공유되는 멀티프로그래밍 시스템에서 확실히 문제가 된다. busy wait는 또한 다른 프로세스가 생산적으로 일할 수 있는 CPU 사이클을 낭비한다.(6.6절에서, 우리는 이 busy wait를 회피하는 방법을 보여주겠다.)

뮤텍스 락의 종류는 또한 **spin-lock**이라고도 불리는데 왜냐하면 프로세스가 락을 기다리는 동안 "spin"하기 때문이다. 스핀락은 장점이있는데, 락을 기다리는 프로세스에 대해서 컨텍스트 스위치를 필요로하지 않기 때문이다. 멀티 코어 시스템의 특정 상황에서, 스핀락은 락킹보다 좋은 선택이 될수 있다. 만약 락을 짧은 시간동안 잡고있으면, 한 스레드는 임계영역에 도달하기 전까지 "spin"하면 되기 때문이다. 현대의 멀티코어 컴퓨터 시스템에서는, 스핀락은 많은 운영체제에서 쓰이고 있다.

## 6.6 Semaphores

뮤텍스 락은 동기화 도구러서 가장 간단한 툴이다. 이번 절에서는, 우리는 뮤텍스락과 비슷하게 작동하지만 프로세스의 활동을 정교하게 동기화하는 단단한 툴을 보여주겠다.

**Sempahpore** S는 정수변수이고, 아토믹 명령어 wait()와 siganl()로만 접근이 가능하다. 세마포는 다익스트라에 의해서 소개되었고, wait() 명령어는 P(to to test)였고 signal()은 V(to increment)였다. 

wait()의 정의는 다음과 같다.
```cpp
wait(S){
    while(S<=0) ; 
        //busy wait
    S--;
}
```

siganl()의 정의는 다음과 같다.
```cpp
signal(S){
    S++;
}
```

wait()와 siganl() 명령어는 모두 아토믹하게 세마포의 정수 값을 수정한다. 즉, 한 프로세스가 세마포의 값을 변경하면, 다른 프로세스는 세마포의 값에 접근이 불가능하다. 추가적으로 wait(S)의 경우에는, 인티저 S의 값 측정(S <= 0)과 S--는 모두 인터럽트 없이 실행되어야 한다. 우리는 6.2.2에서 어떻게 이런 명령어가 작동하는지 보겠다.

### 6.6.1 세마포 사용

운영체제는 카운팅과 이진 세마포를 구분한다. **Counting sempahore**의 갑값은 제한되지 않았다. **Binary semaphore**는 0과 1만 가능하다. 그러므로, 이진 세마포어는 뮤텍스락과 비슷하다. 실제로, 뮤텍스락을 제공하지 않는 시스템에서, 바이너리 세마포어는 뮤텍스 대신에 제공된다.

카운팅 세마포는 주어진 리소스의 인스턴스 수를 제어하는 것으로 사용된다. 세마포어는 가용한 리소스의 수로 초기화된다. 리소스를 사용하기를 원하는 프로세스는 세마포어에 대해서 wait() 명령어를 실행한다. 프로세스가 리소스를 해제하면, 그것은 signal()명령어를 실행한다. 세마포어의 수가 0이되면 모든 리소스는 사용되는 중이라는 것이다. 그 후에, 리소스를 사용하기를 원하는 프로세스는 카운트가 0보다 커지기전까지는 블락될 것이다.

우리는 또한 세마포어를 동기화 문제 해결을 위해 사용할 수 있다. 예를 들어서, 두개의 동시 작동중인 프로세스를 생각해보자. S1 구문을 가진 P1과 S2 구문을 가진 P2이가 있다. S2는 S1이 완료된 이후에 실행될 필요가 있다. 우리는 이것은 P1과 P2가 공유 세마포어 synch를 0으로 초기화해서 가지면 된다.
P1에서는 다음의 구문을 넣는다.
```cpp
S1;
signal(synch);
```

P2에서는 다음의 구문을 넣는다.
```cpp
wait(synch);
S2;
```

synch가 0으로 초기화되었기 떄문에, P2는 오직 P1이 S1을 끝내고 signal()을 실행한 후에만 실행가능하다.

### 6.6.2 세마포어 구현

뮤텍스의 구현은 busy-waiting때문에 고통 받았다. 설명한 wait()와 signal() 세마포어 명령어의 정의도 같은 문제를 가진다. 이 문제를 극복하기 위해서, 우리는 wait()와 signal() 명령의 정의를 다음과 같이 수정했다. 프로세스가 wait() 명령어를 실행중이고 세마포어의 값이 양수가 아니라면, 그것은 반드시 기다려야한다. 그러나, busy wait를 하는 것보다는, 프로세스가 알아서 멈춘다. 정지 명령어는 프로세스를 세마포어 관련된 대기 큐에 넣고, 프로세스의 상태는 대기가 된다. 그때, 컨트롤은 CPU 스케쥴러에게 넘어가고 다른 프로세스를 실행하게 선택한다.

정지된 프로세스는, 세마포어 S를 기다리고, 다른 프로세스가 signal 명령어를 실행할때까지 재시작된다. 프로세스는 프로세스를 대기 상태에서 레디 상태로 바꾸는 wakeup() 명령어로 재시작된다. 프로세스는 레디큐에 재배치된다.(CPU는 현재의 실행 프로세스를 새로운 레디프로세스로 바꿀지 안바꿀지는, 스케쥴링 알고리즘에 달려있다.)

세마포어를 다음의 정의로 구현하려면, 세마포어는 다음과 같이 정의한다.

```cpp
typedef struct{
    int value;
    struct process *list;
}semaphore;
```

각 세마포어는 정수 값과 프로세스의 리스트를 가진다. 프로세스가 세마포어를 기다릴떄, 그것은 프로세스의 리스트에 추가된다. signal() 명령어는 대기 프로세스의 리스트에서 하나를 지우고 프로세스를 깨운다.

이제 wait() 세마포어 명령어는 다음과 같이 정의된다.
```cpp
wait(semaphore *S){
    S->value--;
    if(S->value < 0){
        add this process to S->list;
        sleep();
    }
}
```

signal() 세마포어 멸령어는 다음과 같이 정의 된다.

```cpp
signal(semaphore *S){
    S->value++;
    if(S->value <= 0){
        remove a process P from S->list;
        wakeup(P);
    }
}
```
sleep() 명령어를 실행한 명령어는 정지된다. wakeup(P) 명령어는 정지된 프로세스 P의 실행을 재개한다. 이 두가지 명령어는 운영체제 기본 시스템 콜로 주어진다. 

이 구현에서는, 세마포어 값이 음수가 될 수 있다, 반면에 busy wait하는 고전 세마포어는 세마포 값이 결코 음수가 되지 않는다. 만약 세마포어의 값이 음수가 되면, 그것의 크기는 현재 대기중인 세마포어의 개수이다. 그러므로 wait() 명령어의 감소 구현부분의 위치를 바꾸는 것은 문제가 되지 않는다.

대기 프로세스의 리스트는 각 PCB를 연결한 대기 프로세스의 리스트로 구현될 수 있다. 각  세마포어는 정수 값을 가지고 PCB의 리스트 포인터를 가진다. 바운디드 웨이팅을 확실히하는 리스트에서 프로세스를 추가하거나 없애는 방법은 FIFO 큐이고, 세마포어는 큐의 헤드와 테일 포인터를 가진다. 일반적으로, 리스트는 어떤 큐잉 전략을 가질수 있다. 세마포어의 정확한 사용은 특정한 큐잉 전략과는 상관이 없다.

말했듯이, 세마포어 명령어가 아토믹하게 실행되는 것은 중요하다. 우리는 반드시 동시에 두개의 프로세스가 같은 세마포어에서 wait()와 signal() 할 수 없다는 것을 보장해야한다. 이것은 임계영역 문제이고, 싱글프로세서에서야 간단하게 두 명령어 사이에 인터럽트 방지만 해두면 된다. 이 싱글 프로세서 환경에서의 구조 행동은, 한번 인터럽트가 방지되면, 다른 프로세스의 명령어는 끼어들수없다. 오직 현재 실행중인 프로세스는 인터럽트가 켜지고 스케쥴러가 컨트롤을 얻을때만 가능하다.

멀티코어 환경에서, 인터럽트들은 반드시 모든 프로세싱 코어에서 꺼져야한다. 다른 경우에는 다른 프로세스의 명령어가 끼어들 수 있다. 모든 코어의 인터럽트를 끄는 것은 힘들고 성능을 낮추게 될 것이다. 그러므로, SMP 시스템은 반드시 다른 대안을 제공해야한다.(CAS나 스핀락을 통한 wait()와 signal() 아토믹화)

우리는 확실하게 wait()와 signal() 명령어의 정의에서 busy-waiting을 없앴다고 확신할 수 없다. 오히려, 우리는 busy wait을 엔트리 부분에서 임계영역으로 옮겼을 뿐이다. 더 나아가서, 우리는 wait()와 signal() 명령어의 임계 영역을 제한 했고, 이 부분들은 짧다(만약 제대로 코딩되었고, 10개 이상의 명령어가 없으면). 그러므로, 임계 영역은 거의 활동중이지 않고, busy waiting이 아주 짧은 시간동안 가끔 일어난다. 전체적으로 다른 상황이 임계영역이 얼마나 긴가(분 또는 시간) 또는 거의 계속 차지하는가는 상황마다 다르다. 이런 경우에는, busy waiting은 매우 비효율적이다.

## 6.7 모니터

비록 세마포어들이 프로세스 동기화를위해서 편리하고 효과적인 메커니즘을 제공하지만, 그들을 부적절하게 사용하는 것은 이런 에러가 특정 실행의 순서에서만 일어나기 때문에 감지하는 것은 어렵다.

우리는 생산자-소비자 문제에서 count의 에러 예시를 보았다. 이런 예제에서, timing 문제는 거의 일어나지 않고, 그때의 count 값은 매우 합리적으로 보인다. 그럼에도 불구하고, 해결법은 마땅치 않다. 이런 이유로 뮤텍스 락과 세마포어가 먼저 소개된 것이다.

불행히도, 이런 타이밍 에러는 뮤텍스락 또는 세마포어에서 여전히 일어난다. 이런 것을 설명하기 위해서, 우리는 임계영역 문제에서의 세마포어 해결법을 복습하겠다. 모든 프로세스가 mutex라는 이진 세마포어를 공유하고 1로 초기화 되어있다. 각 프로세스는 반드시 wait(mute)를 임계영역에 들어가기전에 실행하고 그후에 signal(mutex)를 실행한다. 만약 이 순서가 보이지 않으면, 두개의 프로세스는 동시에 임계영역에 들어갈 수 있다. 다음으로, 우리는 일어날 수 있는 몇가지 문제를 보겠다. 이런 어려움은 만약 싱글 프로세스가 제대로 일하지 않아도 생길 것이다. 이런 상황은 결백한 프로그램 에러 또는 비협조적인 프로그래머에 의해서 일어난다. 

```cpp
signal(mutex);

// critical section

wait(mutex);
```

- 프로그램이 세마포어 mutex가 실행하는 wait()와 signal의 순서를 교환한다고 가정하겠다. 이런 상황에서, 몇가지 프로세스는 임계영역을 동시에 실행해서, 상호배제를 어길 수 있다. 이 에러는 오직 여러개의 프로세스가 임계영역에 동시에 활동중일때 발견된다. 이 상황은 항상 생기지는 않는다.

```cpp
wait(mutex);

// critical section

wait(mutex);
```

- 프로그램이 signal(mutex)를 wait(mutex)로 교체한다고 가정하겠다. 이런 경우에는 두번째 wait() 콜에 따라서 세마포어는 영구적으로 차단된다.

- 프로세스가 wait(mutex) 또는 signal(mutex)를 생략했다고 가정하자. 이런 경우에는 상호배제가 어겨지거나 프로세스는 영구히 블락된다.

이런 예제는 프로그래머가 임계영역 문제를 해결하려고 할때 쉽게 실수하는 부분이다. 이런 문제를 해결하는 방법은 간단한 동기화 툴을 high level 언어를 설계할때 포함시키는 것이다. 이 절에서는, 우리는 기본적인 하이레벨 동기화 건설법인 **monitor**를 보겠다.

### 6.7.1 모니터 사용법

**abstract data type(ADT)**는 특정 데이터에서 작동할 함수를 캡슐화시키는 것이다. *monitor type*은 프로그래머가 모니터 내에서 상호배제를 제공하는 명령어의 집합을 포함하는 ADT이다. 모니터 타입은 또한 이런 변수를 실행하는 함수의 바디를 따라서, 그 타입의 인스턴스의 상태를 정의하는 변수를 선언한다. 모니터 타입의 대표는 다양한 프로세스에 의해서 직접 사용이 불가능한 것이다. 그러므로 모니터에서 정의된 함수는 오직 모니터에서 지역적으로 선언된 변수와 그것의 파라미터에 의해서만 접근이 가능하다. 비슷하게, 모니터의 로컬 변수는 오직 그것의 로컬 함수에 의해서만 접근이 가능하다.

모니터의 개략적인 구조는 다음과 같다.
```cpp
monitor monitor name
{
    /*shared variable declarations */

    function P1( . . . ) {
        . . .
    }
    function P_n( . . . ) {
        . . .
    }
    initialization_code( . . . ){
        . . .
    }
}
```
모니터 구조는 오직 하나의 프로세스만 모니터 안에서 활동중일 수 있게 보장한다. 결과적으로, 프로그래머는 동기화 제약을 explicit하게 할 필요가 없어진다. 그러나 모니터 구조는 몇가지 동기화 구조를 모델링하기에는 충분하지 않을 수가 있다. 이런 목적으로, 우리는 추가적인 동기화 메커니즘을 제공받는다. 이 메커니즘은 condition 구조에게 제공받는다.  동기화에 딱 맞춘 구조는 하나 또는 많은 type condition 변수를 쓰는 것을 필요로 한다. 
`condition x, y;`
condition 변수를 깨우는 것은 오직 wait()와 signal() 변수이다. 
`x. wait()`는 프로세스가 다른프로세스가 `x.signal();`을 실행하기 전까지 정지하는 것을 의미한다.

x.signal() 변수는 정지된 프로세스를 재개시킨다. 만약, 아무런 프로세스가 정지되어있지 않으면, signal() 명령어는 아무런 효과가 없다. 즉, x의 상태는 명령어가 한번도 실행된적없는 상태와 같다. 이 명령어와는 반대로, 세마포에서의 signal() 명령어는 항상 세마포어의 상태에 영향을 미쳤다.

이제 x.signal() 명령어가 프로세스 P에 의해서 실행되고, 정지된 프로세스 Q가 x 상태와 관련이 되어있다고 가정하자. 명백하게, 만약 정지된 Q가 그것의 실행을 재개하면, 시그널링 프로세스 P는 반드시 기다려야한다. 다른 경우에는 P와 Q가 모니터 안에서 동시에 활동중이다. 두개의 프로세스가 동시에 그들의 실행을 진행할수 있지만, 두가지 가능성이 존재한다.

1. Signal and wait. P는 Q가 모니터를 떠나거나 다른 condition을 기다릴 때까지 기다린다.
2. Signal and continue. Q는 P가 모니터를 떠나거나 다른 condition을 기다릴 때까지 기다린다.

각 옵션을 채택하는 합리적인 주장이 존재한다. 한 측면에서는, P가 이미 모니터에서 실행중이면 *signal-and-continue*가 합리적이다. 반면에, 만약에 스레드 P가 지속되고, 그때 time Q가 재개되면, 어떤 Q가 대기중인지의 논리적 condition은 더이상 기다리지 않는다. 두가지 선택의 절충안은 존재한다. 만약 스레드 P가 signal 명령을 실행하면 그것은 모니터를 즉시 떠나고 Q는 즉시 재개된다.

자바, C# 같은 비슷한 메커니즘을 이용해서 concurrency 지원을 제공하는 많은 프로그래밍 언어들이 이 절에서 설명한 모니터를 포함한다.

## 세마포를 이용한 모니터 구현

```cpp
monitor ResourceAllocator
{
    bool busy;
    condition x;

    void acquire(int time){
        if(busy) x.wait(time);
        busy = true;
    }

    void release(){
        busy = false;
        x.signal();
    }

    initilization_code(){
        busy = false;
    }
}
```

세마포를 이용한 모니터링 메커니즘의 구현을 고려하겠다. 각 모티너에서, 이진 세마포어 mutex(1로 초기화됨)는 상호배제를 제공한다. 프로세스는 반드시 모니터에 들어가기 이전에 wait(mutex)를 실행해야한다. 프로세스는 반드시 모니터를 떠나기전에 signal(mutex)를 해주어야한다.

우리는 signal-and-wait 구조를 구현에 사용하겠다. 시그널링 프로세스는 재개된 프로세스가 떠나거나 기다릴때까지 반드시 기다려야하고, 0으로 초기화된 추가적인 이진 세마포어 next가 소개된다. 시그널링 프로세스는 그들 자체적으로 정지하기 위해서 사용할 수 있다. 정수 변수 next_count는 또한 next에서 정지된 프로세스의 수를 세는 것을 제공한다.

각 외부 함수 F는 다음과 같이 대체된된다.
```cpp
wait(mutex);
    . . .
    body of F
    . . .
if(next_count > 0) signal(next);
else signal(mutex);
```
모니터에서의 상호배제는 보장된다.

우리는 이제 어떻게 condition 변수가 구현되는지 보겠다. 각 컨디션 x를 위해서, 우리는 이진 세마포어 x_sem과 정수 변수 x_count를 소개한다.(둘다 0으로 초기화되었다.) 명령어 x.wait()는 다음과 같이 구현된다.
```cpp
x_count++;
if(next_count > 0) signal(next);
else signal(mutex);
wait(x_sem);
x_count--;
```

명령어 x.signal()은 다음과 같이 구현된다.
```cpp
if(x_count > 0){
    next_count++;
    signal(x_sem);
    wait(next);
    next_count--;
}
```
이 구현은 모니터의 정의에 걸맞다.(Hoare과 Brinch-Hansen이 만든 bibilographical note) 몇가지 경우에는, 이 구현은 불필요하지만, 효율성의 막대한 증가가 가능하다. 우리는 6.27의 예제로 남겨두겠다.

### 6.7.3 모니터에서 프로세스 재개

우리는 모니터에서의 프로세스의 재시작 순서를 보겠다. 만약 몇가지 프로세스가 condition x에서 정지되어있으면, x.signal() 명령어가 몇가지 프로세스에서 실행되면, 어떤 정지된 프로세스가 다음으로 실행되어야 할까? 한가지 간단한 방법은 FCFS 순서이고, 그 프로세스에서 가장 길게 기다른 것이 제일 먼저 실행된다. 많은 상황에서, 이런 간단한 스케쥴링 구조는 적절치 않다. 이런 상황에서는, **conditional wait**가 사용될 수 있다. 이 구조는 `x.wait(c)`를 가진다. c는 wait() 명령어가 실행되었을때 평가되는 정수 표현이다. c의 값은, **priority number**라고 불리며 정지된 프로세스의 이름과 함께 저장된다. x.signal()이 실행되면, 가장 낮은 우선도 숫자를 가진 것이 재개된다.

이 새로운 메커니즘을 설명하기 위해서, 우리는 경쟁하는 프로세스에서 무엇을 바꾸어야할까? 각 프로세스는, 리소스의 할당을 요청할때, 리소스를 사용할 최대시간을 알려준다. 모니터는 가장 짧은 할당 시간을 가진 프로세스에게 리소스를 할당한다. 

리소스에 접근하는 프로세스는 반드시 다음과 같은 질문을 해야한다. R은 모니터의 인스턴스이다.
```cpp
R.acquire(t);
 . . . 
    access the resource;
 . . .
R.release();
```

불행히도, 모니터 개념은 앞서는 접근 시퀀스를 관측한다고 보장하지 않는다. 다음과 같은 문제가 생긴다.
- 프로세스는 리소스에 접근 허용을 얻기전에 접근할수도있다.
- 프로세스는 그것이 리소스에 접근을 허용받고 해제하지 않을 수도 있다.
- 프로세스는 요청하지 않은 리소스를 해제할 수도 있다.
- 프로세스는 같은 요청을 동시에 2번할 수도 있다.

세마포의 사용과 같은 어려움이 있고, 이런 어려움은 모니터 구조를 처음 개발할때 태생적으로 나타난다. 이전에, 우리는 세마포의 정확한 이용을 고민했다. 이제는, 우리는 높은 레벨 프로그래머 정의 명령어의 정확한 사용을 고민해야한다.(심지어, 컴파일러는 도와줄 수 없다.)

한가지 해결책은 리소스 접근 명령어를 객체안에 포함하는 것이다. 그러나 이 방법을 사용하는 것은 우리가 만든 것이 아니라 빌트인 모니터 스케쥴링 알고리즘을 따르는 것이다.

프로세스가 적절한 순서를 관찰하는 것을 보장하기 위해서, 우리는 모니터 객체를 쓰는 모든 프로그램과 그것의 관리된 리소스를 확인해야한다. 우리는 반드시 이 시스템의 두가지 condition을 확인해야한다. 첫째, 유저 프로세스들은 반드시 모니터에서 그들의 콜을 만들어야한다. 두번쨰, 우리는 비동기 프로세스가 모니터에 의해서 간단하게 상호배제를 무시하게 해야한다. 그리고 액세스 프로토콜 없이 공유된 리소스를 직접 접근하게 한다. 이 두 상황은 시간-의존 에러가 생기는 것과 스케쥴링 알고리즘이 사라지지 않는 것을 보장한다.

비록 이 관찰이 매우 작은 곳이나 정적 시스템에서는 먹히지만, 그것은 큰 시스템이나 동적 시스템에서는 먹히지 않을 수 있다. 액세스-컨트롤 문제는 17장에서 해결하도록 하겠다.

## 6.8 Liveness

임계영역을 조직화하는 동기화 도구 사용의 한가지 결론은 임계 영역에 프로세스가 대기 없이 들어가게 시도하는 것이다. 6.2절에서 보았듯이, 우리는 3가지 기준을 보았다. 기약 없는 대기는 progress와 bounded-waiting을 어기는 것이다. 

**Liveness**는 시스템이 반드시 그들의 실행 라이프 사이클동안 프로세스가 Progress를 만들도록 만족해야하는 성질이다. 기약없이 기다리는 프로세스 같은 상황은 "liveness failure"라고 할 수 있다.

여러가지 실패의 방법이 있다. 그러나 대부분은 낮은 성능과 반응성 때문이다. 가장 간단한 예는 무한 루프이고, 만약 프로세스가 긴 시간동안 루프에 빠지는 것이다. 동시 프로그래밍에서 뮤텍스락과 세마포는 자주 이런 실패에 빠진다. 이 절에서는, 우리는 이 실패에 빠지는 두가지 상황에 대해서 알아보겠다.

### 6.8.1 데드락

대기 큐를 가진 세마포의 구현은 두개 또는 여러개의 프로세스가 대기하고 있는 프로세스에 의해서 이벤트가 일어날 때 일어난다. signal() 명령어의 실행인 것이다. 이런 상황이 일어나면, 이 프로세스들은 데드락된다.

P0, P1이 두가지 세마포 S, Q를 1로 가진다고 생각하겠다.
| P0 | P1 |
| --- | --- |
| wait(S); | wait(Q); |
| wait(Q); | wait(S)); |
| . . . | . . . |
| siganl(S); | signal(Q); |
| siganl(Q); | signal(S); |

프로세스 P0가 wait(S)를 실행하고 P1이 wait(Q)를 실행했다고 가정하자. P0가 wait(Q)를 실행하면, P1이 signal(Q)를 할때까지 기다려야한다. 비슷하게 P1이 wait(S)를 실행하면, 그것은 P0가 signal(S)를 할떄까지 기다려야한다. 이 signal() 명령어가 실행될 수 없기 때문에, P0와 P1이 데드락된다.

우리는 *집합 내의 모든 프로세스가 다른 프로세스에 의해서만 실행되는 이벤트를 기다리는*  프로세스의 집합을  데드락 상태라고 말한다. 여기서 "events"는 뮤텍스와 세마포 같은 리소스의 해제와 획득을 보통 고려한다는 것이다. 다른 타입으로 인해서 데드락이 생길수도 있지만, 8장에서 보겠다. 그곳에서는 데드락 문제를 해결하는 다양한 메커니즘을 설명하고 liveness failure의 다른 형태도 볼 것이다.

### 6.8.2 Priority Inversion

스케쥴링 챌린지는 높은 우선 순위의 프로세스가 현재 낮은 우선순위의 프로세스에 의해서 접근되는 커널 데이터를 수정하거나 읽어야할때 필요하다. 커널 데이터는 보통 보호되기 때문에, 높은 우선 순위의 프로세스는 낮은 우선순위의 프로세스가 리소스를 끝마칠때까지 기다려야한다. 상황은 만약 낮은 우선 순위의 프로세스가 높은 우선 순위의 다른 프로세스에게 선점당할때 더욱 복잡해진다.

예로서는, 3가지 프로세스 L, M, H가 있다고 가정하자. 우선 순위의 순위는 L < M < H이다. H가 세마포 S를 필요로 하고, 현재 L에 의해서 사용되고있다. 일반적으로, 프로세스 H는 L이 리소스 S를 다 쓰기를 기다려야한다. 그러나, 프로세스 M이 가동가능해졌고, 프로세스 L을 선점하려고 한다. 비직접적으로, 낮은 순위의 프로세스(M)은 리소스 S를 포기하기 위해서 프로세스 H가 얼마나 기다렸는지에 영향을 받는다.

liveness 문제는 **priority inversion**이라고도 한다. 그리고 이문제는 두개이상의 우선도가 섞일때 문제가 생긴다. 일반적으로, 우선도 역전은 **priority-inheritance protocol** 구현으로 회피 가능하다. 이 프로토콜에 따르면, 높은 우선순위를 필요로 하는 리소스 접근을 하려는 모든 프로세스는 그들이 리소스 사용을 마칠때까지 높은 우선순위를 내재해야한다. 그들이 끝나면, 그들의 우선도는 원래의 값으로 돌아간다. 이 예제에서, priority-inheritance protocol은 프로세스 L이 일시적으로 프로세스 H의 우선도를 가지게 허용한다. 그럼으로서 프로세스 M이 그것을 선점하는 것을 방지한다. 프로세스 L이 리소스 S의 사용을 마치면, 그것은 H로 얻은 우선도를 포기하고 그것의 원래 우선도를 얻는다. 리소스 S가 가용해지면, 프로세스 M이 아닌 H는 다음에 실행된다.

> https://www.microsoft.com/en-us/research/people/mbj/just-for-fun/

## 6.9 Evaluation

우리는 임계 영역 문제를 해결하기 위한 몇가지 동기화 툴을 이용했다. 정확한 구현과 사용을 하면, 이 툴들은 상호배제, liveness problem을 확실하게 해결할 수 있다. 현대 멀티 코어 컴퓨터 구조의 성능을 높인 병렬 프로그램의 성장은 동기화 툴의 성능에 대한 관심을 주게했다. 어떤 툴을 사용할지 구별하는 것은 벅찬 챌린지이다. 이 절에서는, 우리는 어떤 특정 동기화 도구를 사용할지 결정하는 몇가지 간단한 전략을 보였다.

6.4절에서의 하드웨어 솔루션은 로우 레벨이고 뮤텍스 락같은 다른 동기화 도구를 건설하는 기반이 되었다. 그러나, 최근에 락의 대리비를 필요로 하지 않으면서 레이스 컨디션을 방지하는 **lock free** 알고리즘을 위한 CAS 명령어를 사용하는 연구가 있다. 비록, 이 락 프리 해결법이 낮은 대리비와 scale하는 능력으로 인해서 인기를 얻고있지만, 알고리즘은 개발하고 테스트하기 어렵다.(이 장의 마지막에, 우리는 lock-free 스택의 정확도에 대해서 이야기 하겠다.)

CAS 기반의 접근은 낙관적으로 보인다. 너는 낙관적으로 먼저 변수를 업데이트하고 다른 스테드가 동시에 변수를 업데이트한다면 충돌 감지를 사용할 것이다. 만약 그렇다면, 너는 그것이 충돌없이 성공할떄까지 반복적으로 재도전할 것이다. 상호배제 락은 반대로 비관적인 전략으로 보인다. 너는 다른 스레드가 동시에 변수를 업데이트하려하면, 너는 업데이트 이전에 비관적으로 락을 얻게할려고한다. 

다음의 가이드라인은 다양한 충돌 부하를 가진 CAS 기반 동기화와 전통적인 동기화(세마포, 뮤텍스락)의 차이점이다.

- Uncontended - 비록 두가지 옵션 모두 빠르지만, CAS 보호는 전통적인 동기화보다 빠를 것이다.
- Moderate contention - CAS 보호는 빠르다. 전통적인 동기화보다 매우 빠르다.
- High Contention - 매우 다투는 부하에서, 전통적인 동기화는 CAS 동기화보다 빠르다.
  
온건한 다툼은 특히 평가하는 것이 흥미롭다. 이 시나리오에서, CAS 명령어는 대부분의 시간을 성공하고, 그것이 실패하면, 그것은 다음 것으로 넘어가기 이전에 계속 루프를 반복할 것이다. 이와 대비해서, 뮤텍스락은 충돌하는 락의 획득이 복잡해보이고 시간을 쓸 것이다. 스레드를 정지하는 코드 패스와 그것을 대기 큐에 넣고 다른 스레드와 컨텍스트 스위치를 일으킨다.

레이스 컨디션을 일으키는 메커니즘의 선택은 시스템 성능에 영향을 받기도 한다. 예를 들어서, atomic 변수는 전통적인 락보다 훨씬 가볍고, 카운터 같은 단일 공유 변수를 업데이트하기에는 뮤텍스나 세마포어보다 적합하다. 우리는 이것을 락이 짧은 시간동안 유지되는 멀티프로세스 시스템에서 스핀락이 사용되는 무분에서 보기도 했다. 보통은, 뮤텍스락은 세마포어보다 간단하고 적은 대리비를 필요로 하고 임계영역 접근을 보호하는 바이너리 세마포어보다 더 선호된다. 그러나 제한된 수의 리소스를 쓸때는, 카운팅 세마포는 뮤텍스락보다 선호된다. 비슷하게, reader-writer 락은 뮤텍스락보다 선호되고, 높은 단계의 병렬을 허용한다.(즉, 다양한 리더이다.)

모니터와 컨디션 변수 같은 높은 레벨의 툴의 매력은 그들의 사용에서의 간단함과 편함이다. 그러나 이런 툴은 심각한 대리비를 요구하고, 그들의 구현에 따라서 높게 다투는 상황에서는 측정이 좋지 않다.

다행히도, 측정가능하고, 효율적인 툴이 병렬 프로그래밍의 수요에 따라서 연구가 증가하고 있다. 몇가지 예제는 다음을 포함한다.
- 컴파일러가 더욱 효율적인 코드를 생산하게 한다.
- 병렬 프로그래밍을 지원하는 언어를 개발한다.
- 현재 존재하는 라이브러리와 APi의 성능을 향상시킨다.

다음 장에서, 우리는 운영체제와 API가 여기서 배운 동기화 도구를 어떻게 구현했는지 보겠다.