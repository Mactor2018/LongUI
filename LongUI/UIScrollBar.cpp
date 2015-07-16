﻿#include "LongUI.h"


// UIScrollBar 构造函数
inline LongUI::UIScrollBar::UIScrollBar(pugi::xml_node node) noexcept: 
Super(node), m_uiAnimation(AnimationType::Type_LinearInterpolation) {
    // 降低渲染优先级以保证最后渲染
    if (!this->priority) {
        force_cast(this->priority) = Priority_AfterMost;
    }
    // 修改
    if (node) {
        wheel_step = LongUI::AtoF(node.attribute("wheelstep").value());
        m_uiAnimation.duration = LongUI::AtoF(node.attribute("aniamtionduration").value());
        register const char* str = nullptr;
        if ((str = node.attribute("aniamtionduration").value())) {
            m_uiAnimation.duration = LongUI::AtoF(str);;
        }
        if ((str = node.attribute("aniamtiontype").value())) {
            m_uiAnimation.type = static_cast<AnimationType>(LongUI::AtoI(str));
        }
    }
    m_uiAnimation.start = m_uiAnimation.end = m_uiAnimation.value = 0.f;
}



// 刷新前
void LongUI::UIScrollBar::BeforeUpdate() noexcept {
    // 垂直?
    if (this->type == ScrollBarType::Type_Vertical) {
        m_fMaxRange = this->parent->height;
        m_fMaxIndex = m_fMaxRange - this->parent->visible_size.height;
        // 检查上边界

        // 检查下边界
    }
    // 水平?
    else {
        m_fMaxRange = this->parent->width;
        m_fMaxIndex = m_fMaxRange - this->parent->visible_size.width;
        // 检查左边界

        // 检查右边界
    }
    // TODO: 更新滚动条状态
    return Super::Update();
}

// 设置新的索引位置
void LongUI::UIScrollBar::SetIndex(float new_index) noexcept {
    // 阈值检查
    new_index = std::min(std::max(new_index, 0.f), m_fMaxIndex);
    m_uiAnimation.start = m_uiAnimation.value = m_fIndex;
    m_uiAnimation.end = new_index;
    m_uiAnimation.time = m_uiAnimation.duration;
    m_pWindow->StartRender(m_uiAnimation.time, this->parent);
    m_bAnimation = true;
}

// 设置新的索引位置
void LongUI::UIScrollBar::set_index(float new_index) noexcept {
    new_index = std::min(std::max(new_index, 0.f), m_fMaxIndex);
    // 不同就修改
    if (new_index != m_fIndex) {
        m_fIndex = new_index;
        // 修改父类属性
        UISB_OffsetVaule(this->parent->offset.x) = -new_index;
        this->parent->DrawPosChanged();
        this->parent->AfterChangeDrawPosition();
        // 刷新拥有着
        m_pWindow->Invalidate(this->parent);
    }
}



// do event 事件处理
bool  LongUI::UIScrollBar::DoEvent(const LongUI::EventArgument& arg) noexcept {
    // 控件消息
    if (arg.sender) {
        switch (arg.event)
        {
        /*case LongUI::Event::Event_FindControl:
            if (arg.event == LongUI::Event::Event_FindControl) {
                // 检查鼠标范围
                assert(arg.pt.x < this->width && arg.pt.y < this->width && "check it");
                arg.ctrl = this;
            }
            __fallthrough;*/
            /*case LongUI::Event::Event_SetFocus:
            __fallthrough;
        case LongUI::Event::Event_KillFocus:
            return true;
        case LongUI::Event::Event_MouseEnter:
            (L"<%S>: MouseEnter\n", __FUNCTION__);
            break;*/
            break;
        case LongUI::Event::Event_MouseLeave:
            m_pointType = PointType::Type_None;
            m_lastPointType = PointType::Type_None;
            return true;
        }
    }
    // 系统消息
    else {
        // 鼠标移上

    }
    return false;
}


// UIScrollBarA 构造函数
LongUI::UIScrollBarA::UIScrollBarA(pugi::xml_node node) noexcept: Super(node), 
m_uiArrow1(node, "arrow1"), m_uiArrow2(node, "arrow2"), m_uiThumb(node, "thumb"){
    // 修改颜色
    if (node) {
        m_fArrowStep = LongUI::AtoF(node.attribute("arrowstep").value());
    }
    // 修改颜色
    else {
        D2D1_COLOR_F normal_color = D2D1::ColorF(0xF0F0F0);
        m_uiArrow1.GetByType<Element::ColorRect>().colors[Status_Normal] = normal_color;
        m_uiArrow2.GetByType<Element::ColorRect>().colors[Status_Normal] = normal_color;
        normal_color = D2D1::ColorF(0x2F2F2F);
        m_uiArrow1.GetByType<Element::ColorRect>().colors[Status_Pushed] = normal_color;
        m_uiArrow2.GetByType<Element::ColorRect>().colors[Status_Pushed] = normal_color;

    }
    // 初始化代码
    m_uiArrow1.GetByType<Element::Basic>().Init(node, "arrow1");
    m_uiArrow2.GetByType<Element::Basic>().Init(node, "arrow2");
    m_uiThumb.GetByType<Element::Basic>().Init(node, "thumb");
    // 检查
    BarElement* elements[] = { &m_uiArrow1, &m_uiArrow2, &m_uiThumb };
    for (auto element : elements) {
        if (element->GetByType<Element::Meta>().IsOK()) {
            element->SetElementType(Element::Meta);
        }
        else {
            element->SetElementType(Element::ColorRect);
        }
        element->GetByType<Element::Basic>().SetNewStatus(Status_Normal);
        element->GetByType<Element::Basic>().SetNewStatus(Status_Normal);
    }
    // 检查属性
    m_bArrow1InColor = m_uiArrow1.GetByType<Element::Basic>().type == Element::ColorRect;
    m_bArrow2InColor = m_uiArrow2.GetByType<Element::Basic>().type == Element::ColorRect;

}
#define LONGUI_FLOAT_OFFSET()


// UI滚动条(类型A): 刷新
void LongUI::UIScrollBarA::Update() noexcept {
    // 先刷新父类
    Super::BeforeUpdate();
    D2D1_RECT_F draw_rect; this->GetContentRect(draw_rect);
    // 双滚动条修正
    if (this->another) {
        // 修改
        UISB_OffsetVaule(draw_rect.right) -= this->another->GetTakingUpSapce();
    }
    m_rtThumb = m_rtArrow2 = m_rtArrow1 = draw_rect;
    register float length_of_thumb, start_offset;
    {
        register float tmpsize = UISB_OffsetVaule(this->width) - BASIC_SIZE*2.f;
        if (this->another) {
            tmpsize -= this->another->GetTakingUpSapce();
        }
        start_offset = tmpsize * m_fIndex / m_fMaxRange;
        length_of_thumb = tmpsize * (1.f - m_fMaxIndex / m_fMaxRange);
    }
    // 修改数据
#if 1
    // 垂直滚动条
    if (this->type == ScrollBarType::Type_Vertical) {
        m_rtArrow1.bottom = m_rtArrow1.top + BASIC_SIZE;
        m_rtArrow2.top = m_rtArrow2.bottom - BASIC_SIZE;
        m_rtThumb.top = m_rtArrow1.bottom + start_offset;
        m_rtThumb.bottom = m_rtThumb.top + length_of_thumb;
    }
    // 水平滚动条
    else {
        m_rtArrow1.right = m_rtArrow1.left + BASIC_SIZE;
        m_rtArrow2.left = m_rtArrow2.right - BASIC_SIZE;
        m_rtThumb.left = m_rtArrow1.right + start_offset;
        m_rtThumb.right = m_rtThumb.left + length_of_thumb;
    }
#else
    // 这段有点长, 使用UISB_OffsetVaule效率可能反而不如if
#endif
    // 刷新
    UIElement_Update(m_uiArrow1);
    UIElement_Update(m_uiArrow2);
    UIElement_Update(m_uiThumb);
    // 刷新
    if (m_bAnimation) {
        m_uiAnimation.Update(m_pWindow->GetDeltaTime());
        this->set_index(m_uiAnimation.value);
        if (m_uiAnimation.time <= 0.f) {
            m_bAnimation = false;
        }
    }
    // 刷新
    return Super::Update();
}

// UIScrollBarA 渲染 
void LongUI::UIScrollBarA::Render(RenderType type) const noexcept  {
    if (type != RenderType::Type_Render) return;
    D2D1_MATRIX_3X2_F matrix; this->GetWorldTransform(matrix);
    // 更新
    D2D1_RECT_F draw_rect; this->GetContentRect(draw_rect);
    // 双滚动条修正
    if (this->another) {
        UISB_OffsetVaule(draw_rect.right) -= this->another->GetTakingUpSapce();
    }
    m_pBrush_SetBeforeUse->SetColor(D2D1::ColorF(0xF0F0F0));
    m_pRenderTarget->FillRectangle(&draw_rect, m_pBrush_SetBeforeUse);
    //
    //this->parent;
    //UIManager << DL_Hint << m_rtArrow2 << endl;

    // 渲染部件
    m_uiArrow1.Render(m_rtArrow1);
    m_uiArrow2.Render(m_rtArrow2);
    m_uiThumb.Render(m_rtThumb);
    // 前景
    auto render_geo = [](ID2D1RenderTarget* const target, ID2D1Brush* const bush,
        ID2D1Geometry* const geo, const D2D1_RECT_F& rect) noexcept {
        D2D1_MATRIX_3X2_F matrix; target->GetTransform(&matrix);
        target->SetTransform(
            D2D1::Matrix3x2F::Translation(rect.left, rect.top) * matrix
            );
        target->DrawGeometry(geo, bush, 2.33f);
        // 修改
        target->SetTransform(&matrix);
    };
    // 渲染几何体
    if (m_bArrow1InColor) {
        D2D1_COLOR_F tcolor = m_uiArrow1.GetByType<Element::ColorRect>().colors[
            m_uiArrow1.GetByType<Element::Basic>().GetStatus()
        ];
        tcolor.r = 1.f - tcolor.r; tcolor.g = 1.f - tcolor.g; tcolor.b = 1.f - tcolor.b;
        m_pBrush_SetBeforeUse->SetColor(&tcolor);
        render_geo(m_pRenderTarget, m_pBrush_SetBeforeUse, m_pArrow1Geo, m_rtArrow1);
    }
    // 渲染几何体
    if (m_bArrow2InColor) {
        D2D1_COLOR_F tcolor = m_uiArrow2.GetByType<Element::ColorRect>().colors[
            m_uiArrow2.GetByType<Element::Basic>().GetStatus()
        ];
        tcolor.r = 1.f - tcolor.r; tcolor.g = 1.f - tcolor.g; tcolor.b = 1.f - tcolor.b;
        m_pBrush_SetBeforeUse->SetColor(&tcolor);
        render_geo(m_pRenderTarget, m_pBrush_SetBeforeUse, m_pArrow2Geo, m_rtArrow2);
    }
    // 前景
    Super::Render(RenderType::Type_RenderForeground);
}


// UIScrollBarA::do event 事件处理
bool  LongUI::UIScrollBarA::DoEvent(const LongUI::EventArgument& arg) noexcept {
    //--------------------------------------------------
    // 获取点击
    D2D1_MATRIX_3X2_F world; this->GetWorldTransform(world);
    D2D1_POINT_2F pt4self = LongUI::TransformPointInverse(world, arg.pt);
    // 控件消息
    if (arg.sender) {
        switch (arg.event)
        {
        case LongUI::Event::Event_MouseLeave:
            this->set_status(m_lastPointType, LongUI::Status_Normal);
            break;
        }
    }
    // 系统消息
    else {
        switch (arg.msg) {
        case WM_LBUTTONDOWN:
            m_pWindow->SetCapture(this);
            // 记录点击点
            m_bCaptured = true; 
            m_fOldPoint = UISB_OffsetVaule(pt4self.x);
            m_fOldIndex = m_fIndex;
            this->set_status(m_pointType, LongUI::Status_Pushed);
            // 检查
            if (m_pointType == PointType::Type_Arrow1) {
                // 左/上移动
                this->SetIndex(m_uiAnimation.end - m_fArrowStep);
            }
            else if (m_pointType == PointType::Type_Arrow2) {
                // 左/上移动
                this->SetIndex(m_uiAnimation.end + m_fArrowStep);
            }
            break;
        case WM_LBUTTONUP:
            this->set_status(m_pointType, LongUI::Status_Hover);
            m_bCaptured = false;
            m_pWindow->ReleaseCapture();
            break;
        case WM_MOUSEMOVE:
            // Captured状态
            if (m_bCaptured) {
                // 指向thumb?
                if (m_pointType == PointType::Type_Thumb) {
                    register auto pos = UISB_OffsetVaule(pt4self.x);
                    register auto rate = 1.f - m_fMaxIndex / m_fMaxRange;
                    this->set_index((pos - m_fOldPoint) / rate + m_fOldIndex);
#ifdef _DEBUG
                    rate = 0.f;
#endif
                }
            }
            //  检查指向类型
            else {
                if (IsPointInRect(m_rtArrow1, pt4self)) {
                    m_pointType = PointType::Type_Arrow1;
                }
                else if (IsPointInRect(m_rtArrow2, pt4self)) {
                    m_pointType = PointType::Type_Arrow2;
                }
                else if (IsPointInRect(m_rtThumb, pt4self)) {
                    m_pointType = PointType::Type_Thumb;
                }
                else {
                    m_pointType = PointType::Type_Shaft;
                }
                // 修改
                if (m_lastPointType != m_pointType) {
                    this->set_status(m_lastPointType, LongUI::Status_Normal);
                    this->set_status(m_pointType, LongUI::Status_Hover);
                    m_lastPointType = m_pointType;
                }
            }
            return true;
        }
    }
    return Super::DoEvent(arg);
}

// UIScrollBarA:: 重建
auto LongUI::UIScrollBarA::Recreate(LongUIRenderTarget* target) noexcept -> HRESULT {
    m_uiArrow1.Recreate(target);
    m_uiArrow2.Recreate(target);
    m_uiThumb.Recreate(target);
    return Super::Recreate(target);
}

// UIScrollBarA: 初始化时
void LongUI::UIScrollBarA::InitScrollBar(UIContainer* owner, ScrollBarType _type) noexcept {
    // 创建几何
    auto create_geo = [](D2D1_POINT_2F* list, uint32_t length) {
        auto hr = S_OK;
        ID2D1PathGeometry* geometry = nullptr;
        ID2D1GeometrySink* sink = nullptr;
        // 创建几何体
        if (SUCCEEDED(hr)) {
            hr = UIManager_D2DFactory->CreatePathGeometry(&geometry);
        }
        // 打开
        if (SUCCEEDED(hr)) {
            hr = geometry->Open(&sink);
        }
        // 开始绘制
        if (SUCCEEDED(hr)) {
            sink->BeginFigure(list[0], D2D1_FIGURE_BEGIN_HOLLOW);
            sink->AddLines(list + 1, length - 1);
            sink->EndFigure(D2D1_FIGURE_END_OPEN);
            hr = sink->Close();
        }
        AssertHR(hr);
        ::SafeRelease(sink);
        return geometry;
    };
    D2D1_POINT_2F point_list_1[3];
    D2D1_POINT_2F point_list_2[3];
    constexpr float BASIC_SIZE_MID = BASIC_SIZE * 0.5f;
    constexpr float BASIC_SIZE_NEAR = BASIC_SIZE_MID * 0.5f;
    constexpr float BASIC_SIZE_FAR = BASIC_SIZE - BASIC_SIZE_NEAR;
    // 水平滚动条
    if (_type != ScrollBarType::Type_Vertical) {
        //
        point_list_1[0] = { BASIC_SIZE_MID , BASIC_SIZE_NEAR };
        point_list_1[1] = { BASIC_SIZE_NEAR , BASIC_SIZE_MID };
        point_list_1[2] = { BASIC_SIZE_MID , BASIC_SIZE_FAR };
        //
        point_list_2[0] = { BASIC_SIZE_MID , BASIC_SIZE_NEAR };
        point_list_2[1] = { BASIC_SIZE_FAR , BASIC_SIZE_MID };
        point_list_2[2] = { BASIC_SIZE_MID , BASIC_SIZE_FAR };
    }
    // 垂直滚动条
    else {
        //
        point_list_1[0] = { BASIC_SIZE_NEAR, BASIC_SIZE_MID };
        point_list_1[1] = { BASIC_SIZE_MID, BASIC_SIZE_NEAR };
        point_list_1[2] = { BASIC_SIZE_FAR, BASIC_SIZE_MID };
        //
        point_list_2[0] = { BASIC_SIZE_NEAR, BASIC_SIZE_MID };
        point_list_2[1] = { BASIC_SIZE_MID, BASIC_SIZE_FAR };
        point_list_2[2] = { BASIC_SIZE_FAR, BASIC_SIZE_MID };
    }
    // 创建
    m_pArrow1Geo = create_geo(point_list_1, lengthof(point_list_1));
    m_pArrow2Geo = create_geo(point_list_2, lengthof(point_list_2));
    return Super::InitScrollBar(owner, _type);
}

// UIScrollBarA: 需要时
void LongUI::UIScrollBarA::OnNeeded(bool need) noexcept {
    UIManager << DL_Hint << this << L'[' << need << L']' << endl;
    m_fTakeSpace = need ? BASIC_SIZE : 0.f;
    // 修改了? changed?
    if (m_bLastNeed != need) {

    }
    m_bLastNeed = need;
}

// UIScrollBarA 析构函数
inline LongUI::UIScrollBarA::~UIScrollBarA() noexcept {
    ::SafeRelease(m_pArrow1Geo);
    ::SafeRelease(m_pArrow2Geo);
}

// UIScrollBarA 关闭控件
void  LongUI::UIScrollBarA::WindUp() noexcept {
    delete this;
}

// 设置状态
void LongUI::UIScrollBarA::set_status(PointType type, ControlStatus state) noexcept {
    BarElement* elements[] = { &m_uiArrow1, &m_uiArrow2, &m_uiThumb };
    // 检查
    if (type >= PointType::Type_Arrow1 && type <= PointType::Type_Thumb) {
        auto& element = *(elements[
            static_cast<uint32_t>(type) - static_cast<uint32_t>(PointType::Type_Arrow1)
        ]);
        UIElement_SetNewStatus(element, state);
    }
}
// create 创建
auto WINAPI LongUI::UIScrollBarA::CreateControl(pugi::xml_node node) noexcept ->UIControl* {
    // 获取模板节点
    if (!node) {

    }
    // 申请空间
    auto pControl = LongUI::UIControl::AllocRealControl<LongUI::UIScrollBarA>(
        node,
        [=](void* p) noexcept { new(p) UIScrollBarA(node); }
    );
    if (!pControl) {
        UIManager << DL_Error << L"alloc null" << LongUI::endl;
    }
    return pControl;
}



// 
/*auto popup_menu() {
    auto menu = ::CreatePopupMenu();
    if (menu) {
        ::DestroyMenu(menu);
    }
}*/

// popup menu
class CUIPopupMenu {
public:
    // ctor
    CUIPopupMenu() noexcept { };
    // dtor
    ~CUIPopupMenu() noexcept { this->Destroy(); }
    // destroy this
    void Destroy() noexcept {
        if (m_hMenu) {
            ::DestroyMenu(m_hMenu);
            m_hMenu = nullptr;
        }
    }
    // create a menu by default
    bool Create() {
        assert(!m_hMenu && "cannot create again!");
        m_hMenu = ::CreatePopupMenu();
        return !!m_hMenu;
    }
    // create a menu by xml string
    bool Create(const char* xml) noexcept {
        assert(!m_hMenu && "cannot create again!");
        m_hMenu = ::CreatePopupMenu();
        return !!m_hMenu;
    }
    // create a menu by xml node
    bool Create(pugi::xml_node node) noexcept {
        assert(!m_hMenu && "cannot create again!");
        m_hMenu = ::CreatePopupMenu();
        return !!m_hMenu;
    }
private:
    // handle to popup menu
    HMENU               m_hMenu = nullptr;
};