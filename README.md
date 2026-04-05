# [Portfolio] Gear Of Three: Multi-Variant Action Framework
> **UE5 StateTree 기반 고성능 AI 및 3가지 액션 장르(Combat/Platforming/SideScrolling) 통합 프레임워크**

본 프로젝트는 하나의 코어 엔진 위에서 **Combat(3D 액션)**, **Platforming(정밀 이동)**, **SideScrolling(2.5D)**이라는 세 가지 서로 다른 게임플레이 문법을 구현하고, Unreal Engine 5의 차세대 AI 프레임워크인 **StateTree**를 심층적으로 활용한 액션 게임 프레임워크입니다.

---

## 🛠 Tech Stack
- **Engine**: Unreal Engine 5.3
- **Language**: C++ (Core Framework), Blueprints (Data & VFX Binding)
- **AI Framework**: **StateTree** (State-Driven AI Logic)
- **Architecture**: Interface-Based Design (Decoupling), Component-Based HP System
- **Input**: Enhanced Input System (Dynamic Mapping Context)

---

## 🎯 Key Technical Highlights (핵심 기술 역량)

### 1. StateTree 기반의 차세대 AI 아키텍처 (`ALeech`)
기존의 Behavior Tree보다 가볍고 상태 전이가 명확한 **StateTree**를 사용하여 복잡한 적 AI 패턴을 구현했습니다. `ALeech` 캐릭터는 군집 공전, 돌진, 원거리 사격 등의 상태를 유기적으로 전환합니다.

- **Flock Orbit Logic**: 다수의 AI가 플레이어를 중심으로 특정 반경을 유지하며 공전하는 궤도 계산 로직 구현.
- **Event-Driven Transitions**: Gameplay Tag 이벤트를 통해 `Orbit -> Rush -> Fall` 등의 상태 전환을 제어하여 로직의 가독성과 성능 최적화.
- **Custom Task/Evaluator**: `STT_Rush`, `STE_TargetSelector` 등 커스텀 태스크를 C++로 작성하여 확장성 확보.

### 2. 멀티 베리에이션 엔진 아키텍처 (Multi-Genre Integration)
서로 다른 물리 법칙과 조작감이 필요한 3가지 모드를 하나의 프로젝트 내에서 모듈화하여 관리합니다.

- **Combat Variant**: 콤보 공격 문자열(Combo String), 홀드 방식의 차징 공격(Charged Attack), 애니메이션 노티파이를 활용한 정밀한 히트박스 판정.
- **Platforming Variant**: 벽 점프(Wall Jump), 더블 점프, 대시(Dash) 및 조작 편의성을 위한 **코요테 타임(Coyote Time)** 물리 구현.
- **SideScrolling Variant**: 2.5D 시점 제한, 소프트 플랫폼(통과 가능한 발판), 점프 패드 등 고전적 기믹의 현대적 구현.

### 3. 인터페이스 기반 데미지 및 컴포넌트 시스템
객체 간 결합도를 낮추기 위해 인터페이스를 적극 활용하여, 어떤 캐릭터라도 즉시 전투 시스템에 편입될 수 있도록 설계했습니다.

- **IMS_Damageable**: `ReceiveDamage` 인터페이스를 통해 공격자가 피격자의 상세 타입을 몰라도 데미지 전달 가능.
- **UMS_HPComponent**: 체력 관리 로직을 컴포넌트로 분리하여 NPC와 플레이어 모두에게 동일한 생존 로직 적용.

### 4. 정밀한 물리 기반 이동 메카닉 (`PlatformingCharacter`)
플랫포머 게임의 핵심인 '손맛'을 위해 엔진의 이동 컴포넌트를 직접 확장하고 물리 계산을 처리했습니다.

- **Wall Jump Logic**: `SphereTrace`를 통해 벽면의 법선(Normal) 벡터를 추출하고, 이를 기반으로 도약 반작용력을 계산.
- **State Packing**: `uint8` 비트 필드를 사용하여 점프 상태(더블 점프, 벽 점프 여부 등)를 메모리 효율적으로 관리.

---

## 🚀 Technical Challenges & Troubleshooting (문제 해결 경험)

### [Challenge] StateTree 공전(Orbit) 상태에서의 부드러운 위치 보정
- **Problem**: AI가 공전 중 플레이어와의 거리가 급격히 변할 때 물리 충돌로 인해 떨림 현상 발생.
- **Solution**: `OrbitPlaneQuat` 쿼터니언을 활용해 목표 궤도를 사전에 계산하고, `FInterpTo`를 통해 위치를 보간하여 물리 엔진과의 충돌을 최소화한 부드러운 군집 이동 구현.

### [Challenge] 멀티 모드 전환 시 입력 컨텍스트 충돌
- **Problem**: 각 베리에이션마다 점프나 공격의 입력 방식이 달라 입력 처리가 꼬이는 문제.
- **Solution**: `Enhanced Input System`의 **Priority** 시스템을 활용. 각 `PlayerController` 진입 시 해당 모드에 맞는 `InputMappingContext`를 동적으로 주입/제거하여 입력 로직을 완전히 격리함.

### [Challenge] 벽 점프 판정의 정밀도 향상
- **Problem**: 단순 라인 트레이스 사용 시 캐릭터의 모서리 부분이 벽에 닿았을 때 판정이 씹히는 현상.
- **Solution**: 캐릭터의 충돌 캡슐 크기에 맞춘 `SphereTrace`로 교체하고, 감지된 벽면의 법선 방향으로 즉시 `LaunchCharacter`를 호출하여 직관적인 조작감 확보.

---

## 📂 프로젝트 구조

- **`Source/GearOfThree/Variant_Combat/`**: 3D 근접 전투 시스템, 애니메이션 콤보 로직, 히트박스 트레이스.
- **`Source/GearOfThree/Variant_Platforming/`**: 고급 이동(벽 점프, 대시) 및 물리 트리거 시스템.
- **`Source/GearOfThree/Variant_SideScrolling/`**: 2.5D 전용 카메라 매니저 및 월드 인터랙션 기믹.
- **`Source/GearOfThree/Leech/`**: **StateTree** 기반 AI 핵심 로직 (공전, 돌진, 사격 태스크).
- **`Source/GearOfThree/Private/Public/`**: 공통 베이스가 되는 `MS_DamageableCharacter`, `HPComponent` 등 핵심 프레임워크.

---

## 💡 종합 성과
본 프로젝트를 통해 **Unreal Engine 5의 StateTree AI 시스템을 실무 수준으로 제어**하는 기술력을 확보했습니다. 특히 **인터페이스 기반 설계**를 통해 서로 다른 세 가지 장르의 액션을 하나의 프레임워크 안에서 성공적으로 통합함으로써, 대규모 프로젝트에서도 유연하게 대응할 수 있는 아키텍처 설계 능력을 증명했습니다.
