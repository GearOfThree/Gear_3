# AI Debug Visualization Portfolio Guide

본 가이드는 StateTree 기반 AI 시스템의 의사결정 과정을 시각화하여 포트폴리오용 스크린샷 및 영상을 최적으로 캡처하기 위한 절차를 안내합니다.

## 1. 에디터 설정 (Setup)

시각화 기능을 활성화하려면 언리얼 에디터에서 다음 설정을 확인하십시오.

1.  **AI 캐릭터 배치**: 맵(예: `Lvl_JunTest` 또는 `Lvl_EnemyTest`)에 `BP_Leech` 또는 `AGearCharacter` 기반 AI를 배치합니다.
2.  **컴포넌트 확인**: 배치된 AI 액터의 디테일(Details) 패널에서 `AIDebugComp`가 추가되어 있는지 확인합니다.
3.  **옵션 활성화**: `AIDebugComp` 설정에서 다음 항목들이 체크되어 있어야 합니다:
    *   `Enable Debug`: 전체 기능 활성화
    *   `Visualize State Tree`: 현재 상태(State) 및 전환 이유 표시
    *   `Visualize EQS`: EQS 쿼리 결과 시각화
    *   `Visualize Analysis`: 거리, 시야(LOS) 등 분석 데이터 표시

## 2. 권장 캡처 시나리오 (Capture Scenarios)

### 시나리오 A: 상태 전환 (State Transition)
AI가 상황에 따라 어떻게 행동을 바꾸는지 보여주는 가장 기본적인 시나리오입니다.
*   **상황**: 리치(Leech)가 공전(Orbit)하다가 플레이어를 발견하고 돌진(Rush)하는 순간.
*   **캡처 포인트**: 
    *   머리 위 텍스트가 `STATE: ORBIT` (Blue)에서 `STATE: RUSH` (Red)로 바뀌는 찰나.
    *   `Reason: Player Targeted`라는 전환 이유가 함께 노출될 때.
*   **포트폴리오 어필**: "이벤트 기반 StateTree 구조를 통해 불필요한 틱을 줄이고, 명확한 조건 하에 상태 전환이 이루어짐을 증명."

### 시나리오 B: EQS 지능적 판단 (Decision Making)
AI가 단순히 추적하는 것이 아니라, 최적의 위치를 '계산'하고 있음을 보여줍니다.
*   **상황**: AI가 엄폐물(Cover)을 찾거나 공격 위치를 선정할 때.
*   **캡처 포인트**:
    *   바닥에 뿌려진 수많은 Red/Green 구체(후보지 점수).
    *   최종 선택된 위치에 생성된 **Cyan 색상의 기둥(Highlight)**.
*   **포트폴리오 어필**: "Environment Query System을 활용하여 전술적 위치를 실시간으로 평가. 점수 기반의 합리적인 AI 판단 로직 구현."

### 시나리오 C: 성능 및 데이터 분석 (Performance & Data)
단순한 비주얼을 넘어 기술적인 깊이를 보여주는 화면입니다.
*   **상황**: 다수의 AI가 활성화된 상태에서 디버그 정보를 전체적으로 조망.
*   **캡처 포인트**:
    *   `Dist`, `LOS`, `Can Attack` 정보가 실시간으로 변하는 모습.
    *   상단에 고정된 `[StateTree Optimized]` 및 성능 지표 텍스트.
*   **포트폴리오 어필**: "실시간 거리 계산 및 시야 판정 로직을 디버그 툴로 시각화하여 개발 효율성 증대. BT 대비 최적화된 시스템 구축."

## 3. 에디터 팁 (Editor Tips)

*   **게임 일시정지 (Pause)**: 캡처하고 싶은 순간에 `Pause` 키(또는 `~` 누르고 `pause`)를 입력하면 DrawDebug 메시지가 유지된 상태에서 구도를 잡을 수 있습니다.
*   **스크린샷 명령어**: `HighResShot 2` 명령어를 사용하면 고해상도 이미지를 얻을 수 있습니다.
*   **슬로우 모션**: `slomo 0.2` 명령어를 사용하면 긴박한 상태 전환 순간을 더 쉽게 포착할 수 있습니다.

---
**Note**: 본 디버그 시스템은 `AIDebugComponent.cpp`의 `GetStateColor` 함수를 통해 상태별 색상을 관리하므로, 필요에 따라 코드를 수정하여 포트폴리오의 톤앤매너에 맞게 색상을 변경할 수 있습니다.
