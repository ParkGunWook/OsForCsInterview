## What we gonna learn

이 장에서 리눅스 운영체제의 깊이있는 분석을 하겠다. 완벽하고 실제의 운영체제를 평가함으로서, 우리는 우리가 말했던 개념들이 어떻게 관련되는지 볼 수 있을 것이다. 이 장에서, 우리는 리눅스의 역사와 개발을 보고 유저와 프로그래머 인터페이스를 살펴보겠다. 리눅스가 제공하는 것은 유닉스 전통에 기반했다. 우리는 또한 이런 인터페이스의 구현과 디자인을 따져보겠다. 리눅스는 빠르게 진화하는 운영체제이다. 이 장에서 2017년에 출시된 리눅스 커널 4.12를 통해서 설명하겠다.

## Objectives

- 리눅스가 끌어온 유닉스 운영체제의 역사와 니룩스 디자인이 기초한 원칙을 살펴보겠다.
- 리눅스 프로세스와 모델을 평가하고 어떻게 리눅스 스레드가 스레드를 스케쥴하고 IPC를 제공하는지 보겠다.
- 리눅스에서의 메모리 관리를 살펴보겠다.
- 리눅스가 어떻게 파일 시스템을 구현하고 I/O 디바이스를 관리하는지 보겠다.

## 20.1 Linux History

리눅스는 유닉스와 매우 닮았다. 실제로, 유닉스 호환성은 리눅스 프로젝트의 주요한 목적중 하나이다. 그러나, 리눅스는 유닉스보다 훨씬 젊다. 그것의 개발은 1991년에 시작했고, 핀란드 대학생, 리누스 토르발즈가 80386 프로세스(인텔이 만든 첫번쨰 PC 호환되는 32bit CPU)를 위한 작지만 자립하는 커널을 만들기 시작했다.

그것의 개발 초기에는, 리눅스 소스 코드는 무료이고 배포에 제한이 최소였다. 결과적으로, 리눅스의 역사는 전세계의 많은 개발자의 합작품이고, 대부분이 인터넷을 통해 이루어졌다. 유닉스 시스템 서비스의 작은 서브셋만 부분적으로 구현된 첫 커널에서, 리눅스는 현대 유닉스 시스템의 모든 기능을 포함하게 성장했다.

초기에, 리눅스 개발은 중심 운영체제 커널을 중심으로 크게 돌았고, 특전을 받은 코어는 모든 시스템 리소스를 관리하고 컴퓨터 하드웨어와 직접적으로 소통했다. 우리는 이 커널보다 더한걸 원했고, 물론, 완전한 운영체제를 생성했다. 우리는 그러므로 리눅스 커널과 완벽한 리눅스 시스템 사이에 구별을 했다. **Linux kernel**은 리눅스 커뮤니티에 의해서 개발된 작은 조각으로부터 모인 소프트웨어의 조각이다. **Linux system**, 우리가 근래에 안것은, 다양한 컴포넌트를 포함하고, 몇몇은 스크래치로부터 쓰였고, 몇몇은 다른 개발 프로젝트로부터 빌리고, 다른 팀과 협동해서 만든 것이다.

기본 리눅스 시스템은 유저와 앱 프로그래밍에 표준 환경이다. 그러나 그것은 가용한 기능 관리의 표준 목적을 강요하지는 않는다. 리눅스가 성숙하면서, 리눅스의 꼭대기에 올릴 기능의 다른 계층의 필요가 생겼다. 이 필요는 다양한 리눅스 배포판에서 맞추어졌다. **Linux distribution**은 리눅스 시스템의 모든 표준 컴포넌트를 포함하고, 초기 설치와 리눅스의 업그레이드와 설치를 관리하고 시스템에서의 다른 패키지를 제거를 단순화하는 관리자 툴이 더해진다. 현대 배포판은 또한 파일 시스템의 관리, 유저 계정의 생성과 관리, 네트워크, 웹브라우저 워드프로세스의 관리를 위한 툴도 포함한다.


### 20.1.1 The Linux Kernel

첫번쨰로 대중에 공개된 리눅스 버전은 0.01이었고, 1991년 3월 14일에 출시했다. 그것은 네트워킹이 없었고, 오직 80386에 적절한 인텔 프로세서와 PC 하드웨어에서만 작동했고, 디바이스 드라이버 지원을 극적으로 제한했다. 가상 메모리 서브시스템은 꽤 일반적이었고 메모리 맵드 파일에 지원은 포함되지 않았다. 그러나, 초기의 생에는 COW와 보호된 주소 공간으로 공유 페이지를 지원했다. 지원하는 파일 시스템은 Minix 파일 시스템이고, 첫번째 리눅스 커널은 Minix 플랫폼과 교차로 개발되었다.

다음 마일 스톤은 1994년 3월 14일에 배포되었다. 이 배포는 리눅스 커널의 빠른 3년의 개발이 끝이 났다. 아마도 가장 큰 단일 기능은 네트워킹이다. 1.0은 유닉스의 표준 TCP/IP 네트워킹 프로토콜을 지원했고, BSD 호환성 소켓 인터페이스를 네트워킹 프로그램을 위해서 지원했다. 디바이스 드라이버 지원은 이더넷으로 IP 또는 모뎀에서의 연결을 위해서 추가되었다.

1.0 커널은 또한 기존 미닉스 파일 시스템의 제한없이 향상되고 새로운 파일 시스템을 포함했고, 그것은 고성능 디스크 접근을 위한 SCSI 컨트롤러를 지원했다. 개발자들은 파일을 스왑과 임의의 파일의 메모리 매핑을 하기 위해서 페이징을 지원하는 가상 메모리 서브시스템으로 확장했다.

추가적인 하드웨어 지원의 범위는 이 배포에서 포함되었다. 비록 여전히 인텔 PC 플랫폼에 제한되었지만, 하드웨어 지원은 플로피 디스크와 CD-ROM 디바이스를 포함해서 지원하게 성장했다.(사운드 카드, 마우스, 국제 키보드 등). 부동 소수점 계산또한 80387 수학 프로세서가 없는 80386유저를 위해서 제공되었다. 시스템 V 유닉스 스타일 **IPC**, 공유 메모리, 세마포어, 메시지 큐가 또한 구현되었다.

이 시점에, 개발은 1.1 커널 스트림을 시작했지만, 1.0을 따라서 다양한 버그 픽스 패치가 배포되었다. 패턴은 리눅스 커널을 위한 표준 넘버링 컨벤션으로 채택되었다. 홀수의 마이너 버전 넘버를 가진 커널들은 1.1 또는 2.5같은 것들은 **development kernels**이다. 짝수 숫자의 마이너 버전 넘버는 안정된 **production kernels**이다. 안정된 커널을 위한 업데이트는 오직 교정하기 위한 버전인 반면에, 개발 커널은 새롭고 상대적으로 테스트되지 않은 기능을 포함했다. 이 패턴은 버전 3까지 유지되었다.

1995년 3월에, 1.2 커널이 배포되었다. 이 배포는 1.0 배포처럼 같은 향상된 기능을 제공했고, 그러나 그것은 새로운 PCI 하드웨어 버스 아키텍처를 포함한 다양한 넓은 범위의 하드웨어를 지원했다. 개발자들은 다른 PC 특화 기능을 추가했고 80386 CPU의 가상 8086 모드를 지원해서 PC 컴퓨터를 위한 DOS 운영체제의 에뮬레이션을 가능하게 했다. 그들은 계정과 방화벽을 지원해서 IP 구현을 업데이트했다. 동적 로드 가능/불가능한 커널 모듈을 위한 단순한 지원 또한 커널 모듈에 제공되었다.

1.2 커널은 마지막 PC 전용 리눅스 커널이다. 리눅스 1.2를 위한 소스 배포는 SPARC, Alpha와 MIPS CPUs를 지원하도록 구현되었지만, 이런 다른 아키텍처의 완전한 통합은 다음 1.2 커널이 배포되기전에는 시작되지 않았다.

리눅스 1.2 배포는 더 넓은 범위의 하드웨어 지원과 존재하는 기능의 완벽한 구현에 집중했다. 새로운 기능들은 그 시점에 개발되고 있었지만, 메인 커널 소스 코드로의 새로운 코드의 통합은 안정된 1.2 커널이 배포되기까지 연기되었다. 결과적으로, 1.3 개발 스트림은 커널에 추가된 새로운 기능의 큰 덕을 보았다.

이 작업은 리눅스 버전 2.0이 배포되는 1996년 6월에 배포되었다. 이 배포는 메이저 버전 넘버 증가를 받았는데, 왜냐하면 두가지 새로운 기능이 추가되었다. 64비트를 포함한 다중 아키텍처 지원과 Symmetric multiprocessing(SMP) 지원이다. 추가적으로, 메모리 관리 코드는 블록 디바이스의 캐싱과 독립적인 파일 시스템 데이터의 통합된 캐시를 제공함으로서 향상되었다. 이 변화의 결과로, 커널은 파일 시스템과 가상 메모리 성능에서 큰 증가를 받았다. 처음으로, 파일 시스템 캐싱은 네트워크 파일 시스템에 확장되었고 쓰기 가능한 맵드 리전또한 지원한다. 다른 주요한 증가는 내부 커널 스레트, 로더블 모듈사이의 의존성을 노출시키는 메커니즘, 수요에 따른 자동적인 로딩, 파일 시스템 한도, 포식스 호환 리얼 타임 프로세스 스케쥴링 클래스가 추가되었다.

1999년의 2.2 버전의 배포에서도 발전은 계속되었다. UltraSPARC 시스템으로의 포트가 추가되었다. 네트워킹은 더 유연한 방화벽, 향상된 라우팅과 트래픽 관리, TCP를 위한 지원으로 향상되었다. 에이콘, 애플, NT 디스크가 읽힐 수 있고, NFS가 새로운 커널 모드 NFS 다이몬으로 향상되었다. 시그널 핸들링, 인터럽트, 이전보다 SMP 성능은 증가하기 위해서 몇몇 I/O들은 정제된 레벨에 락되었다.

2.4와 2.6 배포의 진전은 증가된 SMP 시스템, 파일 시스템 탐색, 메모리 관리와 블럭 I/O 시스템을 향한 향상을 위한 지원이다. 스레드 스케쥴러는 버전 2.6에서 수정되었고, O(1)의 복잡도를 제공한다. 추가적으로, 2.6 커널은 선점이고, 스레드가 커널 모드로 작동할떄에도 선점하게 허용한다.

리눅스 커널 버전 3.0은 2011년에 배포되었다. 주요한 2~3으로의 발전은 리눅스의 20주년을 기념하기 위해서이다. 향상된 가상 지원, 새로운 페이지 라이트 백 기능, 메모리 관리 시스템으로의 기능이 포함되고 그러나 다른 새로운 스케쥴러, Completely Fair Scheduler(CFS)또한 나타났다.

리눅스 버전 4.0은 2015년 4월에 배포되었다. 이 번에는 매우 모호하다. 리눅스 커널 개발자들은 큰 마이너 버전에 지쳤다. 오늘날 리눅스 커널 버전은 배포 순서를 제외하고는 별 의미가 업다. 4.0 커널 시리즈는 새로운 아키텍처, 향상된 모바일 기능, 많은 반복적인 향상을 제공했다. 우리는 이 챕터의 나머지에는 가장 최근의 커널을 다루겠다.

### 20.1.2 The Linux System

앞서 말했듯이, 리눅스 커널은 리눅스 프로젝트의 중심을 만들었지만, 다른 컴포넌트들이 완벽한 리눅스 운영체제를 만들었다. 반면에 리눅스 커널은 리눅스 프로젝트에 특화된 스크래치로부터 전체의 코드를 구성했고, 리눅스를 만든 대부분의 지원 소프트웨어는 리눅스에 독점적이지 않고 유닉스 운영체제에 일반적이다. 특히, 리눅스는 버클리 BSD 운영체제, MIT의 X 윈도우 시스템, GNU의 일부를 개발하는데에 많이 사용했다.

툴의 공유는 두 방향으로 일어났다. 리눅스의 주요한 시스템 라이브러리는 GNU 프로젝트에서 기원했지만, 리눅스 커뮤니티는 생략, 비효율성, 버그를 잡으면서 라이브러리를 크게 향상시켰다. **GNU C complier** 같은 다른 컴포넌트들은 리눅스에서 직접 쓰일정도로 충분히 높은 품질을 가졌다. 네트워크 관리자 툴은 4.3 BSD에서 처음 개발한 코드에서 끌어왔지만, 현재의 BSD는 리눅스에서 코드를 끌어온다. 이 공유의 예시는 인텔 부동 소수점 에뮬레이션 수학 라이브러리와 PC 사운드 하드웨어 디바이스 드라이버를 포함한다.

리눅스 시스템은 인터넷을 넘어서 느슨하게 연결된 개발자들의 협력으로 유지된다. 그리고 특정 컴포넌트의 무결성을 유지하는 책임을 가진 개인과 작은 그룹이 있다. 작은 수의 공용 인터넷 FTP 아카이브 사이트는 사실상의 표준 레퍼지토리이다. **File System Hierachy Standard** 문서는 또한 다양한 시스템 컴포넌트를 건너 호환성을 확신하는 수단으로서 리눅스 커뮤니티에 존재한다. 이 표준은 리눅스 파일 시스템의 전체 레이아웃을 명시한다. 그것은 어떤 디렉토리가 설정 파일, 라이브러리, 시스템 바이너리, 런타임 데이터 파일을 저장해야할지 결정한다.

### 20.1.3 Linux Distributions

이론적으로, 누구든 ftp 사이트로부터 필수 시스템 컴포넌트의 최신 버전을 실행하고 그들을 컴파일링 함으로서 리눅스 시스템을 설치할수 있다. 최근의 리눅스는, 이것이 리눅스 유저가 정확히 해야할 일이다. 리눅스가 성숙하면서, 다양한 개인과 그룹이 이 일을 설치를 위한 표준, 미리 컴파일된 패키지를 제공함으로서 덜 고통스럽게 한다.

이런 모음집, 배포판은 기본적인 리눅스 시스템 이상을 포함한다. 그들은 일반적으로 추가 시스템 설치와 관리 유틸리티 뿐만이 아니라 미리 컴파일되고 설치가 준비된 유닉스 툴(뉴스 서버, 웹 브라우저, 텍스트 프로세싱툴, 심지어는 게임)를 포함한다.

첫번쨰 배포판은 이런 패키지를 단순히 적절한 위치에 모든 파일을 압축해제하도록 제공했다. 현대 배포판의 가장 중요한 기능은, 진보된 패키지 관리이다. 오늘날의 리눅스 배포판은 패키지가 설치되고, 업그레이드되고 고통없이 제거되도록 허용하는 패키지 트래킹 데이터베이스를 포함한다.

SLS 배포판에서, 리눅스의 초기판으로 돌아가면, 완벽한 배포판으로 인식되는 리눅스 패키지의 첫번째 컬렉션이다. SLS는 현제 기대하는 리눅스 배포판의 패키지 관리 툴이 부족하다. **Slackware** 배포판은 전체 품질에서 큰 향상을 얻었다. 그것은 또한 저급한 패키지 관리를 가졌다. 실제로, 그것은 여전히 리눅스 커뮤니티에서 널리 설치된 것중 하나이다.

슬랙 웨어의 출시후에, 많은 상업적 비상업적 리눅스 배포판이 나왔다. **Red Hat**과 **Debian**들은 특별히 유명한 배포판이다. 첫번쨰는 상업적 리눅스를 지원한 회사이고 두번째 무료 소프트웨어 커뮤니티에서 나온 것이다. 다른 리눅스의 상업적 지원 버전은 **Canonical**과 **SuSE**가 있다. 우리가 이제 설치할 리눅스 배포판은 다양하다. 다양한 배포판은 리눅스 배포판들이 호환되는 것을 예방하지 않는다. RPM 패키지 파일 포맷이 대부분의 배포판에서 사용되거나 상업적 앱이 RPM 파일을 수용한 배포판에서만 설치되고 실행하게한다.

### 20.1.4 Linux Licensing

리눅스 커널은 GNU General Public License(GPL)의 버전 2에서 배포되었다. 리눅스는 공적인 도메인 소프트웨어가 아니다. **Public domain**은  저자가 소프트웨어에 대한 저작권을 포기한 것인데, 그러나 리눅스 코드의 저작권은 여전히 코드의 다양한 저자에게 잡혀있다. 리눅스는 *free* 소프트웨어이고, 그러나 사람들이 복사하고 수정하고 그들이 원하는대로 사용하고, 그들의 카피를 팔 수 있다.

리눅스 라이센싱의 주요한 암시는 아무도 리눅스를 사용하거나 리눅스의 파생물을 만들어가 소스코들르 포함하지 않고 파생물을 배포할 수 있다. GPL아래에서 배포된 소프트웨어는 바이너리 전용 프로덕트로 재 배포될 수 없다. 만약 너가 GPL에 의해서 만들어진 소프트웨어를 배포하면, 너는 반드시 소스코드가 바이너리 배포판으로 구별되게 만들어야한다.

## 20.2 Design Principles

그것의 전체 디자인에서, 리눅스는 다른 전통적인, nonmicrokernel 유닉스 구현과 비슷하다. 그것은 유닉스 호환 툴의 완전한 집합을 가진 멀티유저, 멀티태스킹 시스템이다. 리눅스의 파일 시스템은 유닉스 시맨틱에 빠져있고, 표준 유닉스 네트워킹 모델이 완벽히 구현되어있다. 리눅스 디자인의 내부 디테일은 운영체제 개발의 역사에 크게 영향을 받았다.

비록 리눅스가 다양한 플랫폼에서 작동하지만, 그것은 PC 아키텍처에서 독점적으로 개발되었다. 최근 개발의 큰 처리는 잘 펀딩된 개발 또는 연구 시설보다는 개별적인 지지자들에 의해서 실행되었고, 그래서 시작부터 리눅스는 최대한 많은 기능을 제한된 리소스로부터 짜내기 위해서 시도했다. 오늘날, 리눅스는 수백 기가의 메인 메모리와 테라바이트의 디스크 공간과 함께 멀티 프로세스 머신을 행복하게 돌리지만, 그것은 여전히 16MB이하의 램에서 유용하게 돌리기가 가능하다.

PC가 점점 강력해지고, 하드디스크와 메모리가 싸지면서, 초기의, 최소의 리눅스 커널은 더 많은 유닉스 기능을 구현하게 성장했다. 속도와 효율성은 여전히 중요한 디자인 골이지만, 최근의 작업은 3번째 메이저 디자인 골인 표준화에 집중한다. 유닉스 구현의 다양성에 지불한 비용은 한가지를 위해 작성한 소스코드가 다른 곳에서는 정확히 컴파일되거나 실행될 필요가 없다는 것이다. 심지어 같은 시스템 콜이 다른 두 유닉스 시스템에 있으면, 그들은 필수적으로 같은 방식으로 작동하지 않는다. 포식스 표준은 운영체제 행동의 다른 측면에 대한 상세의 집합을 모았다. 일반적인 운영체제 기능와 프로세스 스레드와 리얼타임 명령어 같은 확장판을 위한 포식스 다큐먼트가 있다. 리눅스는 POSIX 다큐먼트를 따르고 두개의 배포판이 공식적인 POSIX 증명을 수행했다.

그것이 프로그래머와 유저에게 표준 인터페이스를 주기 떄문에, 리눅스는 유닉스와 친밀한 누구에게나 약간의 놀라움을 보여준다. 우리는 이런 인터페이스를 여기서는 다루지 않겠다. 고정으로, 리눅스 프로그래밍 인터페이스는 SVR4 유닉스 시맨틱에 부착되었다. 라이브러리의 분리된 집합은 BSD 시맨틱을 구현하는데 가용하다.

많은 다른 표준이 유닉스 세계에 존재하지만, 리눅스의 완전한 증명은 느린데 왜냐하면 증명은 비용을 통해 가용하고, 운영체제의 준수를 증명하는 것은 비용을 포함한다. 그러나, 다양한 범위의 앱의 베이스를 지원하는 것은 운영체제에게 중요하고, 그래서 표준의 구현은 리눅스 개발의 주요한 목적이고, 리눅스는 최근에 포식스 스레딩 확장 Pthreads와 리얼타임 프로세스 컨트롤을 위한 포식스 확장을 지원한다.

### 20.2.1 리눅스 시스템의 컴포넌트

리눅스 시스테은 세가지 메인 바디로 이루어 져있고, 대부분이 전통적인 유닉스 구현과 비슷하다.

1. Kernel. 커널은 가상 메모리와 프로세스 같은 것을 포함한 운영체제의 중요한 관념을 유지하는 책임을 가진다.
2. System libraries. 시스템 라이브러리는 어떤 앱이 커널과 소통하느냐를 통해서 함수의 표준 집합을 정의한다. 이런 함수들은 커널 코드의 완전한 권한을 필요로하지 않는 운영체제 함수의 대부분을 구현한다. 대부분의 중요한 시스템 라이브러리는 `libc`로 알려진 **C library**이다. 표준 C 라이브러리를 제공함으로서, `libc`는 치명적인 시스템 레벨 인터페이스와 리눅스 시스템 콜 인터페이스의 대부분의 유저 모드 사이드를 구현한다.
3. System utilityies. 시스템 유틸리티들은 개별적으로, 특화된 관리 태스크를 수행하는 프로그램들이다. 몇몇 시스템 유틸리티들은 초기화되고 시스템의 몇가지 측면을 설정하기 위해서 한번 실행된다. 다른 **daemons**로 알려진 것들은, 영구적으로 실행되고 다가오는 네트워크 커넥션, 터미널로부터 로그온 요청을 받아들이고 로그 파일을 업데이트하는 태스크를 핸들링한다.

이들을 구별하는 가장 중요한 것은 커널과 나머지이다. 모든 커널 코드는 프로세서의 권한 모드에서 컴퓨터의 모든 물리적 리소스에 완전한 접근을 하면서 실행한다. 리눅스는 이 권한 모드를 **kernel mode**라고 한다. 리눅스에서, 어떠한 유저 코드도 커널안에 빌드될 수 없다. 어떠한 커널 모드에서 실행할 필요가 없는 운영체제 지원 코드는 시스템 라이브러리에 위치하고 **user mode**에서 작동한다. 커널 모드와는달리, 유저 모드는 시스템 리소스의 제한된 서브셋에만 접근할 수 있다.

비록 다양한 모던 운영체제가 그들의커널 내부를 위해서 메시지 패싱 아키텍처를 채택했지만, 리눅스는 유닉스의 역사적인 모델을 유지한다. 커널은 단일로, 하나로 구성된 바이너리로 생성된다. 주요한 이유는 성능이다. 모든 커널 코드와 데이터 구조가 단일 주소 공간에 유지됨으로서, 스레드가 운영체제 함수를 부르거나 하드웨어 인터럽트가 발생할떄 컨텍스트 스위치가 필요하지 않다. 더욱이, 커널은 다양한 서브시스템 사이에서 싼 C 함수 실행과 더욱 복잡하지 않은 IPC로 데이터를 패스하고 요청을 만든다. 단일 주소 공간은 코어 스케쥴링, 가상 메모리 코드뿐만이 아니라 모든 디바이스 드라이버, 파일 시스템, 네트워킹코드를 포함한 모든 커널 코드를 포함한다.

비록 모든 커널 컴포넌트가 이 같은 도가니를 공유하지만, 여전히 모듈을 위한 여지가 있다. 유저 앱이 런타임에 필요한 코드의 조각을 끌어오는 공유 라이브러리를 로드하듯이, 리눅스 커널은 런타임에 동적으로 모듈을 로드할 수 있다. 커널은 어떤 모듀류이 로드되는지를 알 필요가 없다. 그들은 정말로 독립적인 로더블 컴포넌트이다.

리눅스 커널은 리눅스 운영체제의 코어를 형성한다. 그것은 프로세스를 관리하고 스레드를 실행하는데 필요한 모든 기능을 제공하고, 그것은 하드웨어 리소스에 모호하고 보호된 접근을 주는 시스템 서비스를 제공한다. 커널은 운영체제로서 작동할 모든 필요한 기능을 구현한다. 그것의 바램대로, 리눅스 커널에 의해서 제공된 운영체제는 완벽하지 않다. 그것은 유닉스의 많은 기능과 행동이 부족하고, 그것이 제공하는 기능은 유닉스 앱이 그들이 원하는 대로 나타나기를 기대하는 포맷일 필요가 없다. 앱을 실행하는 운영체제 인터페이스는 커널에 의해서 직접적으로 유지되지 않는다. 오히려, 앱은 시스템 라이브러리에 콜을 만들고 필요한 만큼 운영체제 서비스를 부른다.

시스템 라이브러리는 많은 종류의 함수를 제공한다. 가장 간단한 레벨에서, 그들은 리눅스 커널에 시스템 콜을 만드는 앱을 허용한다. 시스템 콜을 만드는 것은 유저 모드에서 커널 모드로 컨트롤을 전송하는 것을 포함한다. 이 전송의 상세는 아키텍처마다 다르다. 라이브러리는 시스템 콜 어규먼트에 신경쓰고, 만약 필요하다면, 이런 어규먼트를 시스템 콜을 만들기위해서 특별하게 나열한다.

라이브러리는 더 복잡한 기본 시스템 콜을 제공할 수도 있다. 예를 들어서, C 언어의 버퍼 파일 핸들링 함수는 시스템 라이브러리에 구현되어있을 수 있다. 그리고 기본 커널 시스템 콜보다 진보된 파일 I/O를 제공한다. 라이브러리는 또한 시스템 콜에 일치하지 않는 루틴을 제공하는데, 소팅 알고리즘, 수학적 함수와 스트링 조정 루틴이다. 유닉스 또는 포식스 앱의 실행을 지원하는 모든 함수들은 시스템 라이브러리에 구현되었다.

리눅스 시스템은 넓은 범위의 유저 모드 프로그램을 포함하고 시스템 유틸리티와 유저 유틸리티도이다. 시스템 유틸리티는 시스템을 시작하고 관리하는데 필요한 모든 프로그램을 포함하고, 네트워크 인터페이스를 셋업하고 시스템으로부터 유저를 제거하고 추가한다. 유저 유틸리티들은 또한 시스템의 기본 명령어에도 필수적인데 그러나 실행하기 위해서 권한을 상승시킬 필요는 없다. 그들은 파일 복사, 디렉토리 생성, 텍스트 파일 수정 같은 단순한 파일 관리 유틸리티를 포함한다. 가장 중요한 유저 유틸리티는 **Shell**이고, 유닉스에서 표준 CLI이다. 리눅스는 많은 쉘을 지원하는데 일반적인 것은 **bourne again shell(bash)**이다.

## 20.3 Kernel Modules

리눅스 커널은 필요에 따라서 필요한 커널 코드의 임의의 일부분을 로드하고 언로드할 능력이 있다. 이런 로드 가능한 커널 모듈은 권한을 가진 커널 모드에서 작동하고 결과는 그들이 작동하는 머신의 하드웨어 능력에 완전히 접근할 수 있다. 이론적으로, 커널 모듈이 할 수 있는 일에 대한 제약은 존재하지 않는다. 다른 것들 사이에서, 커널 모듈은 디바이스 드라이버, 파일 시스템 또는 네트워킹 프로토콜을 구현할 수 있다.

커널 모듈들은 여러가지 이유로 편리하다. 리눅스의 소스 코드는 무료이고, 그래서 커널 코드를 작성하고 싶은 누구나가 수정된 커널을 컴파일하고 새로운 기능을 가지고 리부트할 수 있다. 그러나, 전체 커널을 재컴파일, 재링킹, 재로딩하는 것은 너가 새로운 드라이버를 개발할때 다루기 힘든 사이클을 겪어야한다. 만약 너가 커널 모듈을 사용하면, 너는 새로운 드라이버를 테스트하기 위해서 새로운 커널을 만들 필요가 없다. 드라이버는 그것자체로 컴파일되고 너의 이미 실행중인 커널에 로드된다. 물론, 한번 새로운 드라이버가 작성되면, 그것은 모듈로 배포되고 그래서 다른 유저는 그들의 커널을 다시 빌드할 필요없다는 장점을 가진다.

후자의 경우는 다른 영향을 가진다. 왜냐하면 그것이 GPL 라이센스로 커버되면, 리눅스 커널은 새로운 컴포넌트가 GPL 아래에서 배포되고 그들을 위한 소스코드가 수요에 맞게 만들어진다해도 그것은 등록상표가 있는채로 배포되지 않는다. 커널의 모듈 인터페이스는 서드파티가 그들이 원하는대로 GPL 아래에서 배포되지 않는 디바이스 드라이버 또는 파일 시스템을 배포하고 쓰게 허용한다.

커널 모듈은 리눅스 시스템이 표준 최소 커널로 셋업되게 허용하지만, 디바이스 드라이버는 없다. 유저가 필요한 어떠한 디바이스 드라이버도 스타트업때 시스템에 의해서 명백히 로드되거나 시스템에 의해서 자동으로 로드되고 그것이 사용중이지 않으면 언로드된다. 예를 들어서, 마우스 드라이버는 USB 마우스가 시스템에 플러그인되면 로드되고 언플러그되면 언로드된다.

리눅스의 모듈 지원은 4가지 구성요소를 가진다.

1. **module-management system**은 모듈이 메모리에 로드되고 커널의 나머지와 통신하게 허용한다.
2. **module loader and unloader**는 유저 모듈 유틸리티이고, 모듈 관리 시스템이 메모리에 모듈을 로드하게 할 때 작동한다.
3. **driver registration system**은 모듈이 새로운 드라이버가 가용하다고 나머지 커널에게 알린다.
4. **conflict-resolution mechanism**은 다른 디바이스 드라이버가 하드웨어 리소스를 보존하고 다른 드라이버에 의한 사용으로부터 이런 리소스를 보호한다.

### 20.3.1 Module Management

모듈을 로딩하는 것은 그것의 컨텐츠를 커널 메모리에 단순히 로드하는 것 이상을 필요로한다. 시스템은 반드시 모듈이 만든 커널 심블 또는 엔트리 포인트인 레퍼런스들이 커널의 정확한 주소공간에 업데이트되게한다. 리눅스는 모듈의 로딩을 두가지 섹션으로 나누어서 레퍼런스를 업데이트한다: 커널 메모리의 모듈 코드의 일부의 관리와 참조하게 허용된 모듈인 심벌의 핸들링이다.

리눅스는 커널에 내부 심벌 테이블을 유지한다. 이 심벌 테이블은 후자의 컴파일동안에 커널안에 정의된 심벌의 전체 집합을 포함하지 않는다. 오히려, 심벌은 명백하게 전달되어야한다. 전달된 심벌의 집합은 어떤 모듈이 커널과 상호작용하는지를 잘 정의한 인터페이스로 만들어진다.

비록 커널 함수로부터 심벌을 전달하는 것은 프로그래머에 의해서 명시적인 요청을 필요하지만, 어떠한 특별한 노력이 이 모듈을 임포트하는데 필요하지 않다. 모듈 작성자는 오직 C 언어의 표준 외부 링킹으로 사용될 수 있다. 모듈에 의해서 참조된 어떠한 외부 심벌 그러나 그것에 의해서 선언되지 않은 것은 단순히 컴파일러에 의해서 생성된 최종 바이너리 모듈에서 해결되지 않은 것으로 마크된다. 해결될 필요가 여전히 있는 모든 심벌들은 커널의 심벌 테이블에서 룩업된다. 그리고 현재 실행중인 커널 안의 심벌의 정확한 주소는 모듈의 코드로 대체된다. 만약 시스템 유틸리티가 모듈안의 커널의 모든 레퍼런스를 커널의 심벌 테이블안에서 그들을 찾을 수 없어서 해결할 수 없으면, 모듈은 거절된다. 

모듈의 로딩은 두 단계로 이루어진다. 첫번째, 모듈 로더 유틸리티는 커널에게 모듈을 위한 가상 커널 메모리의 연속적인 공간을 보존하라고 커널에게 물을 수 있다. 커널은 할당된 메모리의 주소를 리턴하고, 로더 유틸리티는 이 주소를 모듈의 머신 코드를 정확한 로딩 주소에 재할당하기 위해서 사용한다. 두번째 시스템 콜은 모듈을 전달하고, 더해서 새로운 모듈이 전달하기를 원하는 심벌 테이블도 전달한다. 모듈 자체는 말 그대로 이전의 할당된 위치에 복사되고, 커널의 심벌 테이블은 로드되지는 않았지만 사용할 가능성이 있는 새로운 심벌을 업데이트한다.

마지막 모듈 관리 컴포넌트는 모듈 요청자이다. 커널은 모듈 관리 프로그램이 연결할 수 있는 통신 인터페이스를 정의한다. 이 연결이 만들어지면, 커널은 프로세스가 현재 로드되지 않은 디바이스 드라이버, 파일 시스템 또는 네트워크를 요청할때마다 관리 프로세스에게 공지하고 그 서비스를 로드할 기회를 매니저에게 줄 것이다. 원래의 서비스 요청은 모듈이 로드되면 완료된다. 매니저 프로세스는 주기적으로 동적으로 로드된 모듈이 여전히 사용중이고 그것이 더이상 활동적으로 필요하지 않을때 모듈을 언로드할지 커널을 문의한다.

### 20.3.2 Driver Registration

한번 모듈이 로드되면, 그것은 커널의 나머지가 어떤 새로운 기능을 제공하는지 알때까지는 메모리의 독립된 영역에 불과하다. 커널은 모든 인식된 드라이버의 동적 테이블을 유지하고 드라이버가 이런 테이블에서 추가되거나 삭제하는 루틴의 집합을 제공한다. 커널은 모듈이 로드되면 모듈의 스타트업 루틴을 부르고 모듈이 언로드되기전에 클린업 루틴을 부른다. 이런 루틴들은 모듈의 기능을 등록할때 실행된다.

모듈은 많은 타입의 기능을 등록한다. 그것은 오직 한가지 타입으로 제한되지 않는다. 예를 들어서, 디바이스 드라이버는 디바이스를 접근하는 두가지 분리된 메커니즘을 원한다. 등록 테이블은 다음을 포함한다.

- Device drivers. 이 디바이스 드라이버들은 캐릭터 디바이스(프린터, 터미널, 마우스), 블록 디바이스(디스크 드라이버)와 네트워크 인터페이스 드라이버를 포함한다.
- File systems. 파일 시스템은 리눅스의 가상 파일 시스템 호출 루틴을 구현하는 것이다. 그것은 디스크에서 파일을 저장하는 포맷을 구현하고, 그러나 그것은 컨텐츠가 리눅스의 `/proc` 파일 시스템 같은 수요에 맞게 컨텐츠가 생성되는 NFS 또는 가상 파일 시스템인 네트워크 파일 시스템에서도 마찬가지이다.
- Network protocols. 모듈은 TCP 같은 전체 네트워킹 프로토콜을 구현하거나 네트워크 방화벽을 위한 패킷 필터링 법칙을 구현한다.
- Binary format. 이 포맷은 실행 가능한 파일의 새로운 타입을 인식, 로딩, 실행하는 방법을 명시한다.

추가로, 모듈은 `sysctl`과 `/proc` 테이블안의 엔트리의 새로운 집합에 동적으로 설정가능한 모듈을 허용하게 등록한다.

### 20.3.3 Conflict Resolution

상업적인 유닉스 구현은 보통 판매자의 하드웨어에서 작동되게 팔린다. 단일 공급자 솔루션의 한가지 장점은 소프트웨어 판매자가 어떤 하드웨어 설정이 가능한지에 대해서 좋은 아이디어를 가지고 있다는 점이다. PC 하드웨어, 그러나, 엄청난 크기의 설정을 가지게되고, 큰 수의 네트워크 카드와 비디오 디스플레이어댑터같은 디바이스 드라이버가 생겨났다. 하드웨어 설정을 관리하는 문제는 모듈러 디바이스 드라이버를 지원할때 더욱 심각해진다. 디바이스의 현재 실행중인 집합은 동적으로 가변적이게 되기 때문이다.

리눅스는 특정 하드웨어 리소스에 접근을 중재하는 것을 돕는 중앙 충돌 해결 메커니즘을 제공한다. 그 목적은 다음과 같다.
- 모듈이 하드웨어 리소스에 접근할때 충돌하는 것을 방지한다.
- **autoprobes**가 존재하는 디바이스 드라이버를 방해하면서 디바이스 드라이버가 자동 탐지 디바이스 설정을 탐색하는 것을 방지한다.
- 같은 하드웨어에 접근하는 다중 드라이버로부터의 분쟁을 해결한다. 예를 들어서, 두개의 병렬 프린터 드라이버와 병렬 IP 네트워크 드라이버가 병렬 포트로 통신하려고 시도하는 것이다.

이런 끝에, 커널은 할당된 하드웨어 리소스의 리스트를 유지한다. PC는 제한된 숫자의 I/O 포트, 인터럽트 라인, DMA 채널을 가진다. 어떤 디바이스 드라이버가 이런 리소스에 접근하려하면, 그것은 커널 데이터베이스로 리소스를 예약하기를 기대한다. 이 필요는 시스템 관리자가 어떤 리소스가 어디에 할당되어야는지 결정하게 한다.

모듈은 그것이 사용하기를 기대하는 하드웨어 리소스를 미리 예약하기 위해서 이 메커니즘을 사용하기를 기대한다. 만약 리소스가 현재 존재하지 않거나 이미 사용중이어서 예약이 거절되면 그것은 모듈이 다음에 뭘 할지 정하게된다. 그것이 초기 시도를 실패하고 만약 계속할 수 없으면 언로드되거나 그것은 다른 대안 하드웨어 리소스를 요청할 수 있다.

## 20.4 Process Management

프로세스는 운영체제 안에서 유저 요청 작업이 서비스되는 기본적인 컨텍스트이다. 유닉스 시스템과 호환되려면, 리눅스는 반드시 유닉스와 비슷한 프로세스 모델을 사용해야한다. 리눅스는 몇가지 주요한 장소에서 유닉스와 다르게 작동한다. 이 절에서, 우리는 전통 유닉스 모델을 다시보고 리눅스의 스레딩 모델을 살펴보겠다.

### 20.4.1 The fork() and exec() Process Model

유닉스 프로세스의 기초적인 원칙은 하나로 합쳐지는 두가지 명령을 두 단계로 분리하는 것이다. 새로운 프로세스의 생성과 새로운 프로그램의 실행이다. 새로운 프로세스는 `fork()` 시스템 콜로 생성되고 새로운 프로그램은 `exec()` 콜 이후에 작동한다. 이 두가지는 확실히 분리된 두가지 함수이다.우리는 새로운 프로그램을 실행하지 않으면서 새로운 프로세스는 `fork()`로 생성할 수 있다. 새로운 서브 프로세스는 정확히 같은 프로그램, 첫번째 프로세스가 작동하는 같은 지점에서 단순히 실행을 계속한다. 이 방법으로, 새로운 프로그램을 실행하는 것은 새로운 프로세스가 먼저 생성되는 거을 필요로하지 않는다. 어떠한 프로세스도 `exec()`로 부를수 있다. 새로운 바이너리 오브젝트는 프로세스의 주소공간에 로드되고 새로운 실행 파일은 존재하는 프로세스의 컨텍스트에서 실행을 시작한다.

이 모델은 단순화의 장점이 있다. 그것은 프로그램을 실행하는 시스템콜안의 새 프로그램의 환경의 모든 디테일을 명시할 필요가 없다. 새로운 프로그램은 단순히 존재하는 환경에서 실행한다. 만약 패런트 프로세스가 새로운 프로그램이 실행하는 환경을 수정하기를 원하면, 그것은 포크하고 그리고 차일드 프로세스에서 오리지널 실행파일을 작동하는 것은 마지막으로 새로운 프로그램을 실행하기 전에 그것이 수정하기를 필요로하는 시스템 콜을 만든다.

유닉스에서, 프로세스는 운영체제가 싱글 프로그램의 싱글 실행의 컨텍스트를 추적하기 위해서 유지해야할 모든 정보를 포함한다. 리눅스에서, 우리는 이 컨텍스트를 특정한 섹션으로 쪼갠다. 넓게, 프로세스 성질은 3가지 그룹으로 나뉜다. Process identity, enviroment, context이다.

#### 20.4.1.1 Process Identity

프로세스 신원은 다음을 주요로 포함한다.

- Process ID(PID). 각 프로세스는 특별한 식별자를 가진다. PID는 앱이 프로세스를 시그널하고 수정하고 대기할때 운영체제가 프로세스를 식별하는데 사용된다. 추가적인 식별자들은 프로세스 그룹과 로그인 세션에 따라서 프로세스를 연관한다.
- Credentials. 각 프로세스는 반드시 시스템 리소스와 파일에 접근하는 프로세스의 권한을 가진 연관된 유저 ID와 하나 이상의 그룹 ID를 가져야한다.
- Personality. 프로세스 개성들은 유닉스 시스템에는 없었지만, 리눅스에서 각 프로세스는 특정 시스템 콜의 시맨틱을 수정할 수 있는 개성 식별자를 가진다. 개성들은 시스템콜이 유닉스의 특정 변형과 호환가능한 요청하는 에뮬레이션 라이브러리에서 사용되었다.
- Namespace. 각 프로세스는 **namespace**라고 불리는 파일 시스템 계보의 특정한 뷰를 가진다. 대부분의 프로세스들은 일반적인 이름공간을 공유하고 그러므로 공유된 파일 시스템 계보에서 작동한다. 프로세스와 그들의 자식들은 다른 이름 공간을 가질 수 있고, 그들의 루트 디렉토리와 마운트된 파일 시스템의 집합인 유일한 파일 시스템 계보이다.

이런 식별자의 대부분은 프로세스의 제한된 컨트롤아래에 있다. 프로세스 그룹과 세션 식별자는 만약 프로세스가 새로운 그룹또는 세션을 시작하기를 원하면 바뀔수 있다. 그러나 프로세스의 1차 PID는 바뀔수 없고 프로세스가 종료될떄까지 유일하게 구별된다.

#### 20.4.1.2 Process Enviroment

프로세스의 환경은 그것의 부모로부터 얻고 두개의 눌 종료 벡터로 구성되어있다.: argument vector와 environment vector이다. **Argument vector**는 단순히 실행중인 프로그램을 깨우는데 사용한 커맨드 라인 어규먼트를 리스트한다. 그것은 전통적으로 프로그램의 이름으로 시작된다. **Environment vector**는 이름있는 환경 변수와 임의의 텍스트 값이 연관된 "NAME=VALUE" 페어의 리스트이다. 환경은 커널 메모리에 잡혀있지 않지만 그것은 프로세스의 스택의 탑에있는 첫번쨰 데이터로서 프로세스의 유저 모드 주소 공간에 저장된다.

어규먼트와 환경 벡터들은 새로운 프로세스가 생성되어도 변하지 않는다. 새로운 자식 프로세스는 부모의 환경을 상속받는다. 그러나, 완벽히 새로운 환경이 새로운 프로그램이 실행될 때 셋업된다. `exec()`를 부르면, 프로세스는 반드시 새로운 프로그램을 위해서 환경을 공급해야한다. 커널은 이런 환경변수를 다음 프로그램에게 넘기고, 프로세스의 현재 환경을 교체해야한다. 커널은 다른경우에 환경과 커맨드 라인 벡터를 단독으로 남겨야한다. 그들의 해석은 유저 모드 라이브러리와 앱에 완전히 달려있다.

한 프로세스에서 다음 프로세스로의 환경 변수의 패싱과 프로세스의 자식에 의한 이런 변수의 상속은 유저 모드 시스템 소프트웨어의 컴포넌트에 정보를 넘길 수 있는 유연한 방법을 제공한다. 다양한 중요한 환경 변수들은 시스템의 관련된 파트에 관습적인 의미를 가진다. 예를 들어서, TERM 변수는 유저의 로그인 세션에 접속하는 터미널의 타입을 이름 짓는 셋업한다. 많은 프로그램들은 이 변수를 어떻게 명령어가 유저의 디스플레이에서 작동하는지 결정한다. (예를 들면, 커서를 움직이고 텍스트 영역을 스크롤링하는 것). 다중 언어 프로그램은 LANG 변수를 다중언어를 지원을 포함하는 프로그램을 위한 시스템 메시지를 보여주는 언어를 결정한다.

환경 변수 메커니즘 커스텀은 각 프로세스 기초에 운영체제를 재단한다. 유저는 그들의 전용 언어를 고르거나 다른 것과 독립적인 그들의 전용 편집기를 선택한다.

#### 20.4.1.3 Process Context

프로세스 신원과 환경 성질은보통 프로세스가 생성되면 셋업되고 프로세스가 종료되기 전까지 변하지 않는다. 프로세스는 만약 필요하다면 그것의 신원의 몇가지 측면을 변경하거나 그것은 그것의 환경을 변화시킨다. 반대로, 프로세스 컨텍스트는 어느 시점에 실행중인 프로그램의 상태이다. 그것은 지속적으로 변한다. 프로세스 컨텍스트는 다음을 포함한다.

- Scheduling context. 프로세스 컨텍스트의 가장 중요한 파트는 그것의 스케쥴링 컨텍스트이다. 이 정보는 스케쥴러가 프로세스를 정지하고 재시작할 필요를 알려준다. 이 정보는 프로세스의 레지스터의 카피를 저장하는 것을 포함한다. 부동 소수점 레지스터들은 개별적으로 저장되고 그들이 필요할때만 복구된다. 스케쥴링 컨텍스트는 또한 스케쥴링 우선도와 프로세스에 전달되기를 기다리는 시그널에 관한 정보도 포함한다. 스케쥴링 컨텍스트의 중요한 부분은 프로세스의 커널 스택인데, 커널 모드 코드에 의해 사용되기 위해 예약된 커널 메모리의 영역이다. 프로세스가 실행중에 일어나는 시스템 콜과 인터럽트은 이 스택을 사용한다.
- Accounting. 커널은 각 프로세스에 의해서 현재 사용되는 리소스와 프로세스의 일생동안 사용된 전체 리소스에 관한 정보를 유지한다.
- File table. 파일 테이블은 열린 파일을 대표하는 커널 파일 구조의 포인터의 행렬이다. 파일 I/O 시스템 콜을 만들때, 프로세스들은 **file descriptor(fd)**라고 알려진 정수를 통해서 파일을 언급한다. 커널은 이 파일에 인덱스를 사용한다.
- File-system context. 파일 테이블이 존재하는 열린 파일을 리스트하는데 비해서, 파일 시스템 컨텍스트는 새로운 파일을 열기위해 요청하는 것을 적용한다. 파일 시스템 컨텍스트는 프로세스의 루트 디렉토리, 현재 실행 디렉토리, 네임스페이스를 포함한다.
- Signal-handler table. 유닉스 시스템들은 다양한 외부 이벤트에 반응하기 위해서 비동기 시그널을 배달한다. 시그널 핸들러 테이블은 특정 시그널에 맞게 취해야할 행동을 정한다. 존재하는 액션은 시그널을 무시하는 것, 프로세스를 종료하는 것, 프로세스의 주소 공간안의 루틴을 실행하는 것을 포함한다.
- Virtual memory context. 가상 메모리 컨텍스트는 프로세스의 개인 주소 공간의 완전한 컨텐츠를 설명한다.

### 20.4.2 Processes and Threads

리눅스는 새로운 실행가능한 이미지를 로딩하지 않고 프로세스를 복제하는 `fork()` 시스템 콜을 제공한다. 리눅스는 또한 `clone()` 시스템콜을 통해서 스레드를 생성하는 능력을 제공한다. 리눅스는 프로세스와 스레드를 구별하지 않는다. 실제로, 리눅스는 프로그램안의 컨트롤의 플로를 언급할때 보통 *process*나 *thread* 보다는 *task*라는 단어를 일반적으로 사용한다. `clone()` 시스템 콜은 그것이 어떤 리소스가 자식과 부모사이에서 공유되는지를 알리는 플래그의 집합을 수락할때를 제외하고 `fork()`와 개별적으로 작동한다. 플래그는 다음을 포함한다.
|flag|meaning|
|---|---|
|CLONE_FS|파일 시스템 정보가 공유된다.|
|CLONE_VM|같은 메모리 공간이 공유된다.|
|CLONE_SIGHAND|시그널 핸들러가 공유된다.|
|CLONE_FILES|열린 파일의 집합이 공유된다.|

그러므로 만약 `clone()`이 위의 플래그들을 전달하면, 부모와 자식 태스크는 공유를 하게된다. `clone()`을 이와 같이 사용하는 것은 다른 시스템에서 스레드를 생성하는 것과 같다. 만약 이 들중에서 어떤 플래그도 세팅되지 않으면, 연관된 리소스들은 공유되지 않고, `fokr()` 시스템 콜의 기능과 비슷한 결과를 가진다.

프로세스와 스레드 사이의 구별 부족은 리눅스가 메인 프로세스 데이터 구조 안에 프로세스의 전체 컨텍스트를 가지고 있지 않기에 가능하다. 오히려, 그것은 독립된 서브 컨텍스트안에 컨텍스트를 가진다. 그러므로, 프로세스의 파일 시스템 컨텍스트, 파일 기술자 테이블, 시그널 핸들러 테이블, 가상 메모리 컨텍스트들은 다른 데이터 구조에 보관된다. 프로세스 데이터 구조는 단순히 다른 구조에 포인터를 포함하고 그래서 프로세스들은 단순히 같은 서브 컨텍스트를 가르키고 레퍼런스 카운트를 증가시킴으로서 쉽게 서브컨텍스트를 공유할 수 있다.

`clone()` 시스템콜에 아규먼트는 어떤 서브 컨텍스트가 공유되고 카피될지를 말해준다. 새로운 프로세스는 늘 새로운 신원과 새로운 스케쥴링 컨텍스트를 가진다. 이런 것들은 리눅스 프로세스의 정수이다. 전달된 어규먼트에 따라서, 커널은 부모의 카피로 초기화된 새로운 서브컨텍스트 데이터 구조를 만들거나 새로운 프로세스가 부모가 사용하는 같은 서브 컨텍스트 데이터 구조를 사용하게할 수 있다.

`fork()` 시스템 콜은 모든 서브컨텍스트를 복사하고 공유하지 않는 특별한 경우의 `clone()`에 불과하다.

## 20.5 Scheduling

스케쥴링은 운영체제 안에서 다른 태스크에 CPU 시간을 할당하는 작업이다. 리눅스는 **preemptive multitasking**을 지원한다. 이런 시스템에서, 프로세스 스케쥴러는 어떤 스레드가 언제 작동할 지 결정한다. 다른 업무들을 건너서 공정성과 성능을 맞추는 결정을 하는 것은 현대 운영체제에서 복잡한 챌린지중에 하나이다.

일반적으로, 우리는 스케쥴링을 유저 스레드의 인터럽팅과 실행으로 생각하지만, 스케쥴링의 다른 측면 또한 중요하다. 다양한 커널 태스크의 실행이다. 커널 태스크들은 실행중인 스레드와 태스크가 요청한 태스크와 커널 자체의 측면에서 내부적으로 작동하는 태스크(리눅스의 I/O 서브시스템에 의해서 생긴 작업)를 포함한다.

### 20.5.1 Thread Scheduling

리눅스는 두가지 분리된 프로세스 스케쥴링 알고리즘을 가진다. 하나는 다중 스레드들 사이에서 공정하고, 선점하는 스케쥴링을 위한 시분배 알고리즘이다. 다른 것은 리얼 타임 태스크를 위해서 디자인 되었고, 적대적인 우선도가 공정성보다 더 중요하다.

루틴 타임 셰어링 태스크에 사용되는 스케쥴링 알고리즘은 커널의 버전 2.6에서 중요한 작업으로 받아들여졌다. 최근의 버전은 전통 유닉스 스케쥴링 알고리즘의 변형으로 실행한다. 이 알고리즘은 SMP 시스템을 위해서 적절한 지원을 제공하지 않았고 시스템이 자람에 따른 태스크의 수에 맞게 확장되지 않았고, 데스크탑과 모바일 디바이스 같은 시스템에서의 상호작용 태스크 사이에서 공정성을 유지하지 않았다. 스레드 스케쥴러는 커널 버전 2.6의 첫번째였다. 버전 2.5는 시스템 내의 태스크수의 관계없이 O(1)의 시간안에 작동할 태스크를 선택하는 스케쥴링 알고리즘을 구현했다. 새로운 스케쥴러는 또한 SMP에 더 좋은 지원을 제공했다.(프로세서 관련성과 로드 밸런싱) 이런 변화들은, 확장성을 증가시켰지만, 상호작용 성능 또는 공정성을 증가시키지는 못했다. 그리고 실제로, 특정 워크로드에서는 이런 문제들이 더욱 심각해졌다. 결과적으로, 스레드 스케쥴러는 리눅스 버전 2.6에서 다시 overhauled되었다. 이 버전은 **Completely Fair Scheduler**를 안내했다.

리눅스 스케쥴러는 두가지 우선도 범위를 가진 선점형, 우선도 기반 알고리즘이다. **real-time**은 0~99를 가지고, **nice value**는 -20~19를 가진다. 적은 `nice` 값은 높은 우선도를 가르킨다. 그러므로, `nice` 값을 올림으로서, 너는 너의 우선도를 낮추고 나머지 시스템에 "친절"해지는 것이다.

CFS는 전통 유닉스 프로세스 스케쥴러로부터 큰 차이를 가진다. 후자에서, 스케쥴링 알고리즘의 중요한 변수는 우선도와 time slice이다. **Time slice**는 스레드가 감당 가능한 시간의 길이이다. 전통적인 유닉스 시스템들은 고정된 타임 슬라이스를 프로세스에게 주고, 아마도 개별적으로 높거나 낮은 우선도 프로세스를 위한 부스트 또는 패널티이다. 프로세스는 그것의 타임 슬라이스의 길이동안 실행하고 높은 우선도의 프로세스는 낮은 우선도의 프로세스 이전에 실행한다. 그것은 많은 non-유닉스 시스템이 작동하는 단순한 알고리즘이다. 이런 단순함은 초기의 타임 셰어링 시스템에서는 잘 작동했지만, 현대의 데스크탑과 모바일 디바이스인에서는 좋은 상호작용 성능과 공정성을 전달하는 것은 적절하지 않다고 증명되었다.

CFS는 전통적인 방식의 타임 슬라이스를 제거한 **fair scheduling**이라고 불리는 새로운 스케쥴링 알고리즘을 제공했다. 타임 슬라이스 대신에, 모든 스레드들은 프로세서의 시간의 일부분을 할당받는다. CFS는 스레드가 실행 가능한 스레드들의 전체 수의 함수로서 얼마나 실행해야할지 계산했다. 시작하기 위해서, CFS는 만약 N개의 실행가능 스레드가 있으면, 각각은 프로세서의 시간의 1/N을 받을 수 있다. CFS는 그리고 각 스레드의 할당량을 그것의 `nice`값으로 가중을 주어서 할당을 조정한다. 고정된 `nice`값을 가진 스레드들은 높은 가중을 받는데 비해서 큰 `nice`값을 가진 것들은 낮은 웨이트를 가진다. CFS는 그러고 러너블 프로세스의 전체 웨이트에 의해서 나누어진 프로세스의 웨이트에 비례한 "time slice"동안 각 스레드를 실행한다.

스레드가 작동하는 시간의 실제 길이를 측정하기 위해서, CFS는 모든 러너블 태스크가 최소한 한번 작동한 시간동안의 시간의 간격인 **target latency**라고 불리는 설정가능한 변수에 의지한다. 예를 들어서, 타겟 레이턴시가 10미리초라고 가정하겠다. 더 나아가 우리가 같은 우선도의 러너블 스레드를 가진다고 가정하겠다. 각각의 스레드는 같은 가중치를 가지고 그러므로 프로세서의 시간의 같은 비중을 받는다. 이런 경우에, 10 미리초의 타겟 레이턴시와 함께, 첫번째 프로세스는 5미리초간 작동하고, 다시 5미리초간 작동한다. 우리가 10개의 러너블이 있으면, CFS는 각각을 반복하기전에 1미리초씩 작동시킨다.

그러나 만약 우리가 1000개의 스레드를 가지면 어떻게 할까? 각 스레드는 오직 1마이크로 초만 작동한다. 스위칭 코스트 때문에, 시간의 짧은 길이를 위한 스레드 스케쥴링은 비효율적이다. CFS는 결론적으로 두번째 설정가능한 변수에 기대하는데, 어떤 스레드든 프로세서에 할당되는 최소 길이의 시간인 **minimum granularity**이다. 모든 스레드들은 그것의 타겟 레이턴시에 관계없이, 최소한 minimum granularity동안 작동할 것이다. 이런 상황에서, CFS는 스위칭 코스트는 러너블 스레드의 수가 심각하게 증가해도 받아들일 수 없을정도로 증가하지 않을 것이다. 이렇게 함으로서, 그것은 공정성이라는 시도를 방해하지 않아야한다. 일반적인 경우에, 그러나, 러너블 스레드의 수는 적당히 유지되고, 공정성과 스위칭 비용은 최대화된다.

공정한 스케쥴링을 위한 스위치와 함께, CFS는 몇가지 방법에서 전통적인 유닉스 프로세스 스케쥴러와 다르게 작동한다. 대부분 알다 싶이, 우리가 보았듯이, CFS는 정적인 타임 슬라이스의 개념을 제거했다. 대신에, 각 스레드는 프로세서의 시간의 일부를 받았다. 얼마나 그 할당이 긴지는 얼마나 많은 다른 스레드들이 실행 가능한지에 달려있다. 이 방법은 선점형, 우선도 기반의 스케쥴링 알고리즘에 내재된 타임 슬라이스에 우선도를 매핑하면서 생긴 많은 문제를 해결했다. 그것은 가능한데, 물론, 클래식 유닉스 스케쥴러를 포기하지 않고 다른 방식으로 이런 문제를 해결했다. CFS는 산출량 성능을 타협하지 않으면서 모바일 디바이스 같은 상호작용 워크로드에서도 잘 작동하는 단순한 알고리즘으로 문제를 해결했다.

### 20.5.3 Kernel Synchronization

그것의 전용 명령어에서 커널을 스케쥴하는 방법은 그것이 스레드를 스케쥴하는 방법과 근본적으로 다르다. 커널 모드 명령어의 요청은 두가지 방법으로 생긴다. 실행중인 프로그램이 운영체제 서비스를 요청하는데, 시스템 콜을 통해서 분명하거나 암시적으로인데, 예를 들어서, 페이지 폴트가 일어날 때이다. 대안으로는, 디바이스 컨트롤러가 CPU가 커널에 정의된 인터럽트를 실행하는데 시작하는 CPU를 야기하는 하드웨어 인터럽트를 전달한다. 

커널의 문제는 모든 이런 태스크들은 같은 내부 데이터 구조에 접근하도록 시도한다는 것이다. 만약 하나의 커널이 몇가지 데이터 구조 접근의 중앙에 있으면, 그때 그 서비스 루틴은 데이터 손상 위험 없이 같은 데이터에 접근하거나 수정할 수 없다. 이 사실은 임계영역(공유된 데이터에 접근하는 코드의 일부분)과 관련되고 그러므로 반드시 동시에 작동하지 않아야한다. 결과적으로, 커널 동기화는 스레드 스케쥴링보다 더많은 것을 포함한다. 커널 태스크가 공유된 데이터의 무결성을 해치지 않으면서 커널 태스크가 작동하게 허용하는 것이 필요하다.

버전 2.6에 앞서서, 리눅스는 비선점형 커널이고, 커널 모드에서 작동하는 스레드는 선점되지 않는다고 의미하는데, 심지어 높은 우선도의 스레드가 실행가능해도이다. 버전 2.6에서는, 리눅스 커널은 완벽히 선점형이 되었다. 이제, 태스크는 커널에서 작동중일때 선점될 수 있다.

리눅스 커널은 스핀락과 세마포를 통해서 커널의 락킹을 제공한다. SMP 머신에서, 근본적인 라킹 메커니즘은 스핀락이고, 커널은 스핀락이 짧은 기간동안 잡히도록 디자인되었다. 단일 프로세서 머신에서, 스핀락은 사용에 적절하지 않고 커널 선점을 활성화하고 비활성화 함으로서 대체된다. 즉, 스핀락을 가지기보다는, 태스크가 커널 선점을 비활성화한다. 태스크가 스핀락을 해제하면, 그것은 커널 선점을 활성화한다.

리눅스는 커널 선점을 활성/비활성화하는데 흥미로운 접근을 사용한다. 그것은 두가지 커널 인터페이스-`preempt_diable()`과 `preempt_enable()`이다. 추가로, 커널은 만약 커널 모드 태스크가 스핀락을 잡고있으면 비선점이다. 이 룰을 강화하려면, 시스템의 각 태스크는 태스크에 의해서 가진 락의 숫자를 알리는 카운터인 `preempt_count` 영역을 포함하는 `thread-info` 구조를 가져야한다.카운터는 락이 획득될때 증가되고 락이 해제될때 감소된다. 만약 현재 실행중인 태스크의 `preempt_count`의 값이 0보다 크면, 그것은 커널을 선점하기에 안전하지 않은데, 이 태스크는 현재 락을 가지고있기 떄문이다. 만약 카운트가 0이면, 커널은 안전하게 인터럽트되고, `preempt_disable()`에 뛰어난 콜이 없다고 가정한다.

커널 선점의 활성과 비활성을 따라서 오직 락이 짧은 기간 획득될 떄 스핀락들은 커널에서 사용된다. 락이 긴시간동안 잡힐때, 세마포어들이 사용된다.

리눅스에 의해서 사용되는 두번쨰 보호 테크닉은 인터럽트 서비스 루틴에서 발생하는 임계영역에 적용된다. 기본적인 툴은 프로세서의 인터럽트 컨트롤 하드웨어이다. 임계영역동안에 인터럽트를 비활성화함으로서, 커널은 공유된 데이터 구조에 동시적 접근의 리스크 없이 그것이 진행하지 못하게 보장한다. 

그러나 인터럽트를 비활성화하는데에 단점이 있다. 대부분의 하드웨어 구조에서, 인터럽트 활성/비활성 명령어는 비싸다. 더 중요한 것은, 인터럽트가 비활성화되면, 모든 I/O들은 정지되고 서비스를 기다리는 모든 디바이스는 인터럽트가 재활성화될 때까지 기다려야한다. 그러므로 성능이 떨어진다. 이 문제를 해결하기 위해서, 리눅스 커널은 긴 임계영역이 그 전체의 기간동안 인터럽트 비활성화 없이 실행하는 것을 허용한다. 이 능력은 네트워킹 코드에서 특히 유용하다. 네트워크 디바이스 드라이버에서의 인터럽트는 전체 네트워크 패킷의 도착을 신호하는데, 인터럽트 서비스 루틴안에 패킷을 해제하고 루트하고 포워드하는 코드의 큰 딜을 결과로한다.

리눅스는 이 구조를 인터럽트 서비스 루틴을 두가지 부분으로 나눔으로서 구현했다. 탑 절반과 바텀 절반이다. *top half*는 재귀적인 인터럽트를 비활성화해서 실행하는 표준 인터럽트 서비스루틴이다. 같은 숫자의 인터럽트들은 비활성화되지만, 그러나 다른 인터럽트들은 작동할 수 있다. 서비스 루틴의 *bottom half*는 바텀 절반이 그들 자체로 인터럽트되지 않도록 확신하는 미니어처 스케쥴러에 의해서 모든 인터럽트를 활성화하고 실행한다. 바텀 하프 스케쥴러는 인터럽트 서비스 루틴이 나갈때마다 자동으로 실행된다.

이 분리는 커널이 그것 자체로 인터럽트되어야할 걱정없이 인터럽트에 맞게 복합적인 프로세싱을 커널이 완료하는 것을 의미한다. 만약 다른 인터럽트가 바텀 하프가 실행하는동안 생겨도, 인터럽트는 같은 바텀 하프가 실행한다고 요청할 수 있지만, 실행은 현재 실행중인 것이 완료될떄까지 연기한다. 바텀 하프의 각 실행은 탑하프에 의해서 인터럽트되지만 비슷한 바텀하프에 의해서 인터럽트되지 않는다.

탑 하프/ 바텀 하프 구조는 일반적인, 중요한 커널 코드가 작동하는 동안 선택된 바텀하프를 비활성화하는 메커니즘에 의해서 완료된다. 커널은 임계영역을 이 시스템을 이용해서 쉽게 구현할 수 있다. 인터럽트 핸들러는 그들의 임계영역을 바텀하프로 코드할 수 있다. 그리고 중요한 커널은 임계영역에 들어가기를 원할때, 그것은 연관있는 바텀 하프를 인터럽트하는 것으로부터 임계영역을 방지한다. 임계영역의 끝에는, 커널은 바텀 하프를 재화렁화하고 임계영역동안 탑하프 인터럽트 서비스 루틴에 의해서 큐된 모든 바텀 하프 태스크를 실행한다.

### 20.5.4 Symmetric Multiprocessing

리눅스 2.0 커널은 분리된 프로세스에서 분리된 스레드가 병렬로 작동하게 허용하는 SMP를 지원하는 첫번째 안정적인 리눅스 커널이다. SMP의 기본 구현은 오직 하나의 프로세스가 한번에 커널 코드를 실행한다는 제약을 가진다.

커널의 2.2 버전에서는, 싱글 커널 스핀락("big kernel lock"이라고도 불리는)이 다중 스레드가 커널안에서 동시에 활성화되게 허용되었다. 그러나, BKL은 락킹 입자의 굵은 레벨만 제공했고, 저조한 확장성을 주었다. 커널의 최신 판은 단일 커널 스핀 락을 다중 락으로 쪼갬으로서 더욱 확정되게 구현했다. 각각은 커널의 데이터 구조의 작은 서브셋만을 보호한다. 이런 스핀락은 20.5.3에서 말했다. 3.0과 4.0 커널은 추가적인 SMP 추가를 제공했고, 프로세서 친화성, 에버 파이너 락킹, 로드 밸런싱 알고리즘과 단일 시스템에서의 수백 수천개의 물리 프로세서를 지원한다.

## 20.6 Memory Management

리눅스에서의 메모리 관리는 두가지 요소가 있다. 첫번쨰는 물리 메모리(페이지, 페이지의 그룹, 램의 작은 블록)를 할당하고 해제하는 것이다. 두번째는 실행중인 프로세스의 주수공간안에 메모리를 매핑하는 가상 메모리를 관리하는 것이다. 이 절에서, 우리는 이런 두가지 요소를 설명하고 `exec()` 시스템 콜에 따라서 새로운 프로그램의 로드 가능한 구성요소를 프로세스의 가상 메모리 안에 가져오는 메커니즘을 평가하겠다.

### 20.6.1 Mangement of Physical Memory

하드웨어 제약 때문에, 리눅스는 물리 메모리를 4가지 **zones**으로 나누었다.
- ZONE_DMA
- ZONE_DMA32
- ZONE_NORMAL
- ZONE_HIGHMEM

이런 존들은 특정한 것을 위한 구조이다. 예를 들어서, 인텔 X86-32 구조에서, 특정 ISA(industry standard architecture) 디바이스는 오직 DMA를 이용해서 16MB 이하의 물리 메모리에 접근할 수 있다. 이런 시스템에서, 첫번째 16MB의 물리 메모리는 ZONE_DMA에 모인다. 다른 시스템들에서, 특정 디바이스들은 64비트 주소 시스템을 지원해도 오직 첫 4GB의 메모리에만 접근이 가능하다. 이런 시스템에서, 물리 메모리의 첫번째 4GB는 ZONE_DMA32로 구성되어있다. ZONE_HIGHMEM은 커널 주소 공간으로 매핑되지 않은 물리 메모리를 참조한다. 예를 들어서, 32비트 인텔 아키텍처에서, 커널은 첫번째 896MB의 주소공간에 매핑되고, 나머지 메모리들은 ZONE_HIGHMEM에 할당되고 *high memory*로 참조한다. 마지막으로, ZONE_NORMAL은 모든 것을 구성하는데, 일반적이고, 정규적으로 매핑된 페이지이다. 아키텍처가 그것의 제약에 따라서 주어진 공간을 가진다. 현대의 64비트 아키텍처는 작은 16MB의 ZONE_DMA(for legacy devices)를 가지고 나머지는 "high memory"없이 ZONE_NORMAL이다.

인텔 x86-32 아키텍처위의 존과 물리 주소의 관계는 아래와 같다.
|zone|physical memory|
|---|---|
|ZONE_DMA| < 16MB|
|ZONE_NORMAL| 16...896MB|
|ZONE_HIGHMEM| > 896MB|

커널은 각 zone마다 여유 메모리 리스트를 유지한다. 물리 메모리를 위한 요청이 도착하면, 커널은 적절한 존을 사용해서 요청을 만족시킨다.

리눅스 커널에서의 1차 물리 메모리 메니저는 **page allocator**이다. 각 존은 그것의 존을 위해서 물리 페이지를 할당하고 해제하는 책임을 가지고 요청에 따라서 물리적으로 연속 적인 페이지의 범위에 할당하는 것이 가능한 할당자를 가진다. 할당자는 버디 시스템을 이용해서 가용한 물리 페이지를 추적한다. 이 구조에서, 할당가능한 메모리의 근처 유닛은 함께 페어된다. 각각의 할당가능한 메모리 구역은 근처 파트너 또는 버디를 가진다. 두개의 할당된 파트너 리전이 해제될 때마다, 그들은 **buddy heap**이라는 큰 영역으로 합쳐진다. 그 큰 리전은 파트너를 가지고, 더큰 여유 리전을 만들기 위해서 합쳐진다. 반대로, 만약 작은 메모리 요청이 존재하는 작은 영역의 할당에의해서 만족되지 않으면, 그때 큰 여유 리전이 두개의 파트너로 나누어져서 요청을 만족한다. 분리된 링크드 리스트들은 각 허용가능한 사이즈의 여유 메모리 영역을 기억하기 위해서 사용된다. 리눅스 아래에서, 이 메커니즘 아래에서 가장 작은 할당 가능한 사이즈는 단일 물리 페이지이다. 16KB의 여유 리전이 있을때 4KB의 요청을 위해서 리전은 원하는 사이즈가 가용해질 떄까지 재귀적으로 쪼개진다.

궁극적으로, 리눅스 커널의 메모리 할당은 시스템 부트 시간동안 메모리의 연속적인 공간을 보존하는 드라이버에 의해서 정적으로 또는 페이지 할당자에 의해서 동적으로 이루어진다. 그러나, 커널 함수는 메모리를 보존하기 위해서 기본 할당자를 사용할 필요가 없다. 몇가지 특화된 메모리 관리 서브시스템들은 그들의 고유 메모리의 풀을 관리하기 위해서 사용된다. 가장 중요한 것은 가상 메모리 시스템이다. `kmalloc()` 가변 길이 할당자이다. 슬랩 할당자는 커널 데이터 구조를 할당하기 위해서 사용된다.; 파일이 소유한 캐싱 페이지를 사용하는 페이지 캐시이다.

리눅스 운영체제의 많은 구성요소들이 요청에 맞게 전체 페이지를 할당할 필요가 있지만, 그들은 적은 블럭의 메모리를 필요로한다. 커널은 요청의 사이즈가 미리 알려져 있지 않고 오직 일부 바이트인 임의의 사이즈 요청을 위한 추가적인 할당자를 제공한다. C 언어의 `malloc()` 함수에 유사한, 이 `kmalloc()` 서비스는 수요에 맞게 전체 물리 페이지를 할당하지만 작은 조각으로 나누어진다. 커널은 `kmalloc()` 서비스를 이용해서 페이지의 리스트를 유지한다. 메모리를 할당하는 것은 적절한 리스트를 결정하고 리스트에서 첫번째 가용한 피스를 가져오거나 새로운 페이지를 할당하고 그것을 쪼개는 것을 포함한다. `kmalloc()` 시스템에 의해서 선언된 메모리 영역들은 그들이 `kfree()`의 적절한 콜에 따라서 명시적으로 해제될떄까지 영구적으로 할당된다. `kmalloc()` 시스템은 메모리 부족에 따라서 이런 리전들을 재할당하거나 재선언할 수 없다.

커널 메모리를 할당하기 위한 리눅스에 의해서 탑재된 다른 전략은 slab 할당이다. **slab**은 커널 데이터 구조를 위한 메모리를 할당하는데 사용되고 하나 이상의 물리적으로 연속된 페이지로 구성된다. **cache**는 하나 이상의 슬랩을 구성한다. 각 커널 데이터 구조를 위한 단일 캐시가 있는데, 예를 들어서, 프로세스 기술자를 대표하는 캐시, 파일 오브젝트의 캐시, 아이노드의 캐시가 있을 것이다. 각 캐시는 캐시가 대표하는 커널 데이터 구조의 인스턴스화가 된 **object**와 함께 있다. 예를 들어서, 아이노드를 대표하는 캐시는 아이노드 구조의 인스턴스를 저장하고, 프로세스 기술자를 대표하는 캐시는 프로세스기술자 구조의 인스턴스를 저장한다.

슬랩 할당 알고리즘은 커널 오브젝트를 저장하기 위해서 캐시를 사용한다. 캐시가 생성되면, 오브젝트들은 캐시에 할당된다. 캐시의 오브젝트 수는 관련된 슬랩의 사이즈에 달려있다. 예를 들어서, 12KB(4KB의 연속된 3개의 페이지) 슬랩은 6개의 2KB 오브젝트를 저장할 수 있다. 초기에, 캐시의 모든 오브젝트들은 `free`로 마크된다. 커널 데이터 구조를 위한 새로운 오브젝트가 필요하면, 할당자는 요청을 만족시키기 위해서 캐실부터 오브젝트를 해제한다. 캐시로 부터 할당받은 오브젝트는 이제 캐시에 `used`로 마크된다.

프로세스 기술자를 대표하는 오브젝트를 위한 슬랩 할당자로부터 커널이 메모리를 요청하는 시나리오를 고려해보겠다. 리눅스 시스템에서, 프로세스 기술자는 1.7KB를 필요로하는 `task_struct` 타입의 구조체이다. 리눅스 커널이 새로운 태스크를 만들면, 그것은 그것의 캐시로부터 `struct task_struct` 오브젝트를 위한 필수 메모리를 요청한다. 캐시는 이미 슬랩에 할당되고 `free`로 마크된 `struct task_struct` 오브젝트를 이용해서 요청을 만족시킨다.

리눅스에서 슬랩은 3가지 상태가 가능하다.

1. Full. 모든 슬랩의 오브젝트들이 used로 마크되었다.
2. Empty. 슬랩의 모든 오브젝트들이 free로 마크되었다.
3. Partial. 슬랩이 두가지 오브젝트 상태를 다 가졌다.

슬랩 할당자는 우선 partial 슬랩에서 여유있는 오브젝트로 요청을 만족한다. 만약 아무것도 없으면, 여유 오브젝트는 empty 슬랩으로부터 할당된다. 만약 empty 슬랩이 없으면, 새로운 슬랩이 연속 물리 페이지에로부터 할당되고 캐시에 할당된다.; 오브젝트를 위한 메모리는 이 슬랩으로부터 할당받는다.

리눅스에서 두가지 주요 서브시스템은 물리 페이지의 관리를 하는 것이다: 페이지 캐시와 가상 메모리 시스템이다. 이런 시스템들은 서로에게 연관이 있다. **page cache**는 파일을 위한 커널의 메인 캐시이고 어떤 블락에 I/O가 실행되는 메커니즘이다. 리눅스와 NFS를 포함한 모든 타입의 파일 시스템은 페이지 캐시를 통해서 그들의 I/O를 실행한다. 페이지 캐시는 파일 컨텐츠의 전체 페이지를 저장하고 블럭 디바이스에 제한되지 않는다. 그것은 또한 네트워크 데이터를 캐시할 수 있다. 가상 메모리 시스템은 각 프로세스의 가상 주고 공간의 컨텐츠를 관리한다. 이런 두시스템들은 데이터의 페이지를 읽는 것은 가상 메모리를 사용해서 페이지 캐시안의 페이지 매핑을 필요로 하기 때문에 가깝게 소통한다. 

### 20.6.2 Virtual Memory

리눅스 가상 메모리 시스템은 각 프로세스가 접근할 수 있는 주소 공간을 유지할 책임이 있다. 그것은 수요에 따라서 가상 메모리의 페이지를 생성하고 디스크로부터 페이지를 로딩하고 그들은 필요에 따라서 디스크 밖으로 스와핑하는 것을 관리한다. 리눅스에서, 가상 메모리 매니저는 프로세스의 주소 공간을 두가지 분리된 시각으로 유지한다. 분리된 리전의 집합과 페이지의 집합으로이다.

주소 공간의 첫번쨰 시각은 가상 메모리 시스템이 주소 공간의 레이아웃에 따라서 가상 메모리 시스템이 받았다는 명령어를 설명하는 논리적 view이다. 이 시각에서, 주소 공간은 겹치지 않는 리전의 집합을 포함하고, 각각의 리전은 연속적이고, 주소공간의 페이지에 얼라인된 서브셋이다. 각 리전은 프로세스의 읽기, 쓰기, 실행을 포함하는 리전의 성질을 정의하고 리전안의 리전과 관련된 파일에 대한 정보에 관한 허가를 실행하는 단일 `vm_area_struct` 구조체에 의해서 내부적으로 만들어진다. 각 주소 공간에 대한 리전은 어느 가상 주소에도 빠르게 룩업하기 위해서 밸런스 트리로 링크되어있다.

커널은 또한 두번쨰, 각 주소공간의 물리적 시각을 유지한다. 페이지 테이블 엔트리는 그것이 디스크 또는 물리메모리에 있든 가상 메모리의 각 페이지의 정확한 현재위치를 식별한다. 물리적 시각은 프로세스가 현재 페이지 테이블에 없는 페이지에 접근을 시도할떄마다 생긴 커널의 소프트웨어 인터럽트 핸들러로부터 실행된 루틴의 집합으로 관리된다. 주소 공간 기술자의 각 `vm_area_struct`는 주어진 가상 메모리 리전을 위한 키 페이지 관리 함수를 구현하는 함수의 테이블을 가르키는 필드를 포함한다. 가용하지 않은 페이지로의 쓰기 또는 읽기 요청은 `vm_area_struct`를 위한 함수 테이블 안의 적절한 핸들러를 실행하고, 그래서 중앙 메모리 관리 루틴들은 각 메모리 리전의 가능한 타입 관리의 디테일을 알지 못한다.

#### 20.6.2.1 Virtual Memory Regions

리눅스는 가상 메모리 리전의 몇가지 타입을 구현한다. 가상 메모리를 특성화하는 한가지 성질은 리전을 위한 페이지가 어디에서 오는지 설명하는 리전을 위한 배킹 스토어이다. 대부분의 메모리 리전들은 파일 또는 nothing으로부터 백된다. nothing으로부터 백된 리전은 가상 메모리 리전의 가장 단순한 타입이다. 이런 리전은 **demand-zero memory**를 대표한다. 프로세스가 이런 리전에서 페이지를 읽으려고 하면, 그것은 단순히 0으로 채워진 메모리의 페이지에 백된다.

파일로부터 백된 리전은 그 파일의 일부위의 viewport로서 행동한다. 프로세스가 그 리전안의 페이지를 접근하려고 시도할때마다, 페이지 테이블은 파일의 적절한 오프셋에 따라서 커널의 페이지 캐시안의 페이지의 주소로 채워진다. 물리 메모리의 같은 페이지는 페이지 캐시와 프로세스의 페이지 테이블에 의해서 사용된다. 그래서 파일 시스템에 의해서 만들어진 어떠한 변화들은 그들의 주소 공간안에 매핑되 파일으로 매핑된 프로세스에 즉각적으로 보여진다. 프로세스의 수는 같은 파일의 같은 리전으로 매핑되고 그들은 그 목적을 위해서 물리 메모리의 같은 페이지를 사용한다.

가상 메모리 리전은 또한 그것의 쓰기에 대한 반응에 의해서 정의된다. 프로세스 주소 공간안으로의 리전의 매핑은 *private*또는 *shared*일 수 있다. 만약 프로세스가 개인적으로 맵드 리전에 쓰려고하면, 페이저는 cow가 프로세스에 로컬로 변화를 유지하는 것이 필요할지 감지한다. 반대로, 공유된 리전의 쓰기는 그 리전에 매핑된 오브젝트의 업데이트를 만들고, 그래서 그 변화는 그 오브젝트에 매핑하는 다른 프로세스가 즉각적으로 보일 것이다.

#### 20.6.2.2 Lifetime of a Virtual Address Space

커널은 두가지 상황에서 새로운 가상 주소 공간을 만든다. 프로세스가 `exec()` 시스템 콜로 새로운 프로그램을 실행할때와 새로운 프로세스가 `fork()` 시스템콜에 의해서 생성될 때이다. 첫번째 케이스는 쉽다. 새로운 프로그램이 실행되면, 프로세스는 새로운, 완벽히 빈 가상 공간을 받는다. 그것은 가상 메모리 리전과 함께 주소 공간을 위치하는 프로그램을 로딩하는 루틴에 달려있다.

두번쨰 케이스에서, `fork()`로 새로운 프로세스를 만드는 것은 존재하는 프로세스의 가상 주소 공간의 완벽한 카피를 만드는 것을 포함한다. 커널은 부모 프로세스의 `vm_area_struct` 기술자를 카피하고, 자식을 위한 새로운 페이지의 집합을 만든다. 부모의 페이지 테이블은 자식에게 직접적으로 카피되고, 각 페이지의 레퍼런스 카운트는 증가한다. 그러므로, fork이후에, 부모와 자식은 그들의 주소 공간 안에서 같은 메모리의 물리 페이지를 공유한다.

특별한 케이스는 복사 명령어가 개인적으로 매핑된 가상 메모리 리전에 도달할때 일어난다. 이런 리전안에 쓰인 부모 프로세스의 어떠한 페이지는 private이고 자식과 부모에 의한 이런 페이지의 연속적인 변화는 다른 프로세스의 주소 공간안의 페이지를 업데이트하지 않아도 된다. 이런 리전의 페이지 테이블이 복사되면, 그들은 읽기 전용으로 세팅되고 COW로 마크된다. 프로세스가 이런 페이지를 더이상 수정하지 않으면, 두 프로세스들은 물리 메모리의 같은 페이지를 공유한다. 그러나, 만약 프로세스가 COW 페이지를 수정하려고 시도하면, 페이지의 레퍼런스 카운트가 확인된다. 만약 페이지가 여전히 공유중이면, 프로세스는 새로운 물리 메모리의 페이지에 페이지의 컨텐츠를 복사하고 그것의 카피를 대신 사용한다. 이 메커니즘은 개인 데이터 페이지가 가능할 떄마다 프로세스 사이에서 공유된다고 확신하고 절대적으로 필요할떄만 복사한다.

#### 20.6.2.3 Swapping and Paging

가상 메모리 시스템에서 중요한 태스크는 물리 메모리로부터 메모리의 페이지를 재할당하는 것이다. 초기의 유닉스 시스템은 이 재할당을 프로세스의 전체 컨텐츠를 한번에 스와핑 아웃함으로서 실행했지만, 현대 유닉스의 버전은 물리 메모리와 디스크 사이에서 가상 메모리의 개별 페이지의 이동인 페이징에 좀더 의존한다. 리눅스는 전체 프로세스 스와핑을 구현하지 않는다. 그것은 새로운 페이징 메커니즘을 독점적으로 사용한다.

페이징 시스템은 두가지 섹션으로 나뉜다. 첫번째, **policy algorithm**은 배킹 스토어에 어떤 페이지를 쓸지 언제 그들을 쓸지 결정한다. 두번쨰, **paging mechanism**은 전송을 실행하고 그들이 다시 필요해지면 데이터를 물리 메모리안으로 다시 백한다.

리눅스의 **pageout policy**는 표준 clock algorithm(second-chance)의 변형된 버전을 사용한다. 리눅스에서, 다중 패스 클락이 사용되는데, 모든 페이지는 클락의 각 패스에 맞게 조정된 *age*를 가진다. 에이지는 페이지의 젊음의 측정이거나 얼마나 페이지가 최근에 활동했는지를 본다. 자주 접근된 페이지는 높은 에이지 값을 얻고, 그러나 자주 쓰지지않는 페이지의 에이지는 각 패스마다 0으로 떨어진다. 이 에이지는 페이저가 LFU 정책에 따라서 페이지 아웃할 페이지를 선택한다.

페이징 메커니즘은 전용 스왑 디바이스와 파티션과 일반 파일에 모두 페이징을 지원한다. 비록 파일로의 스와핑은 파일 시스템에 의해서 발생하는 추가적인 오버헤드때문에 심각히 느리다. 블럭들은 사용된 블럭의 비트맵에 따라서 스왑 디바이스로부터 할당된다. 할당자는 next-fit 알고리즘을 이용해서 향상된 성능을 위해서 2차 저장소 블럭의 연속적인 실행을 위한 페이지 쓰기를 시도한다. 할당자는 현대 프로세스위에서 페이지 테이블 기능의 이용을 통해서 저장소로 페이지 아웃한 사실을 기록한다. 페이지 테이블 엔트리의 page-not-present 비트가 세틍되고, 페이지 테이블 엔트리의 나머지가 어디에 페이지가 쓰일지 식별하도록 채워진다.

#### 20.6.2.4 커널 가상 메모리

리눅스는 그것의 내부는 일정한 모든 프로세스의 가상 주소 공간의 아키텍처 의존 리전이다. 이런 커널 페이지로 매핑되는 페이지 테이블 엔트리는 보호된다고 마크되고, 그래서 그 페이지들은 프로세서가 유저모드에서 실행중일때 보이거나 수정되지 않는다. 이 커널 가상 메모리 공간은 두가지 리전을 가진다. 첫번쨰는 시스템의 모든 가용 페이지의 물리에 페이지 테이블 레퍼런스를 포함하고, 그래서 물리에서 가상 주소로의 간단한 번역이 커널 코드가 실행하면 일어난다. 일반적인 페이지 할당자에 의해서 할당된 모든 페이지를 따라서 커널의 코어는 이 리전에 거주한다.

커널의 예약된 주소 공간의 나머지 부분은 특정한 목저이 없다. 주소의 페이지 테이블 엔트리들은 메모리의 다른 영역을 가르키기 위해서 커널에 의해서 수정된다. 커널은 커널 코드가 이 가상 메모리에서 사용되게 허용하는 기능의 쌍을 제공한다. `vmalloc()`함수는 단일 리전의 가상적으로 연속된 커널 메모리 안에 물리적으로는 아닐수도 있는 메모리의 물리 페이지의 임의의 수를 할당한다. `vremap()` 함수는 가상 주소의 시퀀스를 메모리 맵드 I/O를 위해서 디바이스 드라이버가 사용된 메모리의 영역을 가르킨다.

### 20.6.3 Excution and Loading of User Programs

유저 프로그램의 리눅스 커널의 실행은 `exec()` 시스템 콜에 의한 콜에 의해서 작동한다. 이 `exec()` 콜은 커널이 현재 프로세스 안의 새로운 프로그램을 실행하게 명령하고, 새로운 프로그램의 초기 컨텍스트와 함께 현재 실행 컨텍스트를 완벽히 덮어쓴다. 이 시스템 서비스의 첫번째 작업은 프로세스를 부르는 것이 실행딜 파일에 허용 권한을 명백하게 하는 것이다. 한번 그 문제가 체크되면, 커널은 프로그램을 실행하기 위해서 로더 루틴을 실행한다. 로더는 필수적으로 프로그램 파일의 컨텐츠를 로드할 필요가 없지만, 그것은 최소한 가상 메모리 안으로 프로그램의 매핑을 셋업해야한다.

리눅스에는 프로그램을 로딩하는 단일 루틴이 없다. 대신에, 리눅스는 가능한 로더 함수의 테이블을 유지하고, 그것은 각각에게 이런 함수에게 `exec()` 시스템 콜이 만들어질 때, 주어진 파일을 로딩할 함수를 준다. 로더 테이블이 있는 이유는 커널 1.0과 1.2사이에서, 리눅스의 바이너리 파일을 위한 형식이 바뀌었기 때문이다. 구식의 리눅스 커널은 `a.out` 포맷을 사용했다. 새로운 리눅스 시스템은 현대의 **ELF** 포맷을 사용하는데, 이제 유닉스 구현에 의해서 지원된다. ELF는 `a.out`에 비해서 수많은 장점이 있다. 새로운 섹션이 ELF 바이너리에 복잡하게 하는 로더 루틴 없이 추가될 수 있다. 다중 로더 루틴의 등록을 허용함으로서, 리눅스는 간단하게 ELF와 a.out을 지원한다.

20.6.3.1과 20.6.3.2에서, 우리는 ELF 포맷 바이너리의 로딩과 실행에 집중하겠다. `a.out` 바이너리를 로딩하는 단계는 단순하지만 비슷한 명령이다.

#### 20.6.3.1 Mapping of Porgrams into Memory

리눅스에서, 바이너리 로더는 물리메모리에 바이너리 파일을 로드하지 않는다. 오히려, 바이너리 파일의 페이지들은 가상 메모리의 리전에 매핑된다. 프로그램이 주어진 페이지에 접그낳려 할떄만 페이지는 디맨드 페이징을 이용해서 물리 메모리의 페이지의 로딩을 페이지 폴트 결과한다.

그것은 커널의 바이너리 로더의 책임이 초기 메모리 매핑을 설정하는데 있다. ELF 포맷 바이너리 파일은 몇가지 페이지 얼라인 섹션에 따라서 헤더를 구성한다. ELF 로더는 헤더를 읽고 가상 메모리의 분리된 리전안에 파일의 섹션을 매핑함으로서 작동한다.

주소 공간의 한 끝에서 예약된 리전은 커널에 안착되고, 그것의 고유한 특권있는 가상 메모리의 리전은 일반 유저 모드 프로그램에서 접근 불가하다. 가상 메모리의 나머지는 앱에 가용한데, 파일의 일부를 매핑하거나 앱 데이터에 가용한 리전을 생성하는 커널의 메모리 매핑 함수를 사용한다.

로더의 작업은 프로그램의 실행을 시작하기 위해서 허용하는 초기 메모리 매핑에 셋업된다. 초기화가 필요한 리전은 스택과 프로그램의 텍스트와 데이터 리전을 필요로한다.

스택은 유저 모드 가상 메모리의 탑에 생성된다. 그것은 낮은 숫자의 주소로 아래로 내려간다. 그것은 아규먼트의 카피들과 exec() 시스템 콜에 의해서 주어진 환경 변수를 포함한다. 다른 리전은 가상 메모리의 바텀 끝 근처에 생성된다. 프로그램 텍스트 또는 읽기 전용 데이터를 포함하는 바이너리 파일의 섹션은 쓰기 보호 리전인 메모리에 매핑된다. 쓰기 가능한 초기화된 데이터들은 다음으로 매핑된다. 그리고 어떠한 초기화되지 않은 데이터들은 개별적인 디맨드 zero 리전으로 매핑된다.

이런 고정된 사이즈의 리전은 프로그램이 런타임에 할당된 데이터를 가진 프로그램은 필요한만큼 가변 사이즈 리전이다. 각 프로세스는 포인터 `brk`를 가지고, 그것은 이 데이터 리전의 현재를  가르키고 프로세스들은 그들의 `brk` 리전을 단일 시스템콜 `sbrk()`로 확장하거나 수축할 수 있다.

한번 이런 매핑이 셋업되면, 로더는 프로세스의 프로그램 카운터 레지스터를 ELF 헤더에 저장하고 프로세스는 스케쥴된다.

#### 20.6.3.2 Static and Dynamic Linking

프로그램이 로드되고 실행을 시작하면, 모든 필요한 바이너리 파일의 컨텐츠는 프로세스의 가상 주소공간에 로드된다. 그러나, 대부분의 프로그램은 시스템 라이브러리로부터 함수를 실행할 필요가 있고 이런 라이브러리 함수는 반드시 로드되어야한다. 가장 간단한 방식으로, 필요한 라이브러리 함수를 프로그램의 실행 바이너리 파일에 포함하는 것이다. 이런 프로그램은 정적으로 그것의 라이브러리에 링크되고 정적으로 링크된 실행파일들은 그들이 로드되자말자 실행을 시작할 수 있다.

정적 링킹의 주요한 단점은 모든 생성된 프로그램들이 반드시 같은 시스템 라이브러리 함수를 포함해야한다는 것이다. 그것은 물리 메모리와 디스크 사용의 관점에서 시스템 라이브러리르 메모리에 한번만 로드하는 것이 훨씬 효율적이다. 동적 링킹이 이것을 가능하게한다.

리눅스는 유저 모드에서의 다이나믹 링킹을 특별한 링커 라이브러리를 통해서 구현한다. 모든 동적으로 링크된 프로그램은 작고, 프로그램이 시작할때 불려오는 정적으로 링크된 함수를 포함한다. 이 정적인 함수는 메모리에 링크 라이브러리를 매핑하고 함수가 포함된 코드를 실행한다. 링크 라이브러리는 ELF 바이너리의 섹션에 포함된 정보를 읽음으로서 이런 라이브러리에 필요한 프로그램과 변수의 이름과 함수에 의해서 필요한 동적 라이브러리를 결정한다. 그것은 가상 메모리의 중간에 라이브러리를 매핑하고 이런 라이브러리 안에 포함된 심벌을 레퍼런스를 해결한다. 그것은 공유된 라이브러리가 매핑된 곳에 있다. 그들은 메모리안의 어떤 주소에서도 실행하는 **position independent code**안으로 컴파일된다.