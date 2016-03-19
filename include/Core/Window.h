﻿#pragma once
/**
* Copyright (c) 2014-2016 dustpg   mailto:dustpg@gmail.com
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

// LongUI namespace
namespace LongUI {
    // vector for window
    using WindowVector = EzContainer::PointerVector<XUIBaseWindow>;
    // vector for system window
    using SystemWindowVector = EzContainer::PointerVector<XUIBaseWindow>;
    // config for create window
    namespace Config { struct Window {
        // node of window, maybe null
        pugi::xml_node      node;
        // parent for window, maybe null
        XUIBaseWindow*      parent;
        // postion of window: different top-bottom for popup window,  same for other window
        D2D1_RECT_L         position;
        // height of window
        uint32_t            height;
        // is popup?
        bool                popup;
        // create system window first?
        bool                system;
        // unused
        bool                unused[2];
    };}
    // window for longui
    class XUIBaseWindow {
        // super class
        using Super = void;
    public:
        // string allocator
        using StringAllocator = CUIShortStringAllocator<>;
        // ctor
        XUIBaseWindow(const Config::Window& config) noexcept;
        // dtor
        ~XUIBaseWindow() noexcept;
        // create child window
        auto CreateChildWindow() noexcept ->XUIBaseWindow*;
        // create popup window
        auto CreatePopup(const D2D1_RECT_L& pos, uint32_t height) noexcept ->XUIBaseWindow*;
    public:
        // index of BitArray
        enum BitArrayIndex : uint32_t {
            // exit on close
            Index_ExitOnClose = 0,
            // close when focus killed
            Index_CloseOnFocusKilled,
            // caret in(true) or out?
            Index_CaretIn,
            // do caret?
            Index_DoCaret,
            // in draging?
            Index_InDraging,
            // window rendered in last time, or want to render in this time
            Index_Rendered,
            // new size?
            Index_NewSize,
            // skip render
            Index_SkipRender,
            // do full-render this frame?
            Index_FullRenderThisFrame,
            // count of this
            INDEX_COUNT,
        };
    public:
        // dispose
        virtual void Dispose() noexcept = 0;
        // render: call UIControl::Render
        virtual void Render() const noexcept;
        // update: call UIControl::Update
        virtual void Update() noexcept;
        // recreate: call UIControl::Render
        virtual auto Recreate() noexcept ->HRESULT;
        // move window relative to parent
        virtual void MoveWindow(int32_t x, int32_t y) noexcept = 0;
        // resize window
        virtual void Resize(uint32_t w, uint32_t h) noexcept = 0;
        // set cursor
        virtual void SetCursor(LongUI::Cursor cursor) noexcept = 0;
    public:
        // reset cursor
        void ResetCursor() noexcept { this->SetCursor(Cursor::Cursor_Default); }
        // get window handle
        auto GetHwnd() const noexcept { return m_hwnd; }
        // get top
        auto GetTop() const noexcept { return m_rcWindow.top; }
        // get left
        auto GetLeft() const noexcept { return m_rcWindow.left; }
        // get width of window client zone
        auto GetWidth() const noexcept { return m_rcWindow.width; }
        // get height of window client zone
        auto GetHeight() const noexcept { return m_rcWindow.height; }
        // get viewport
        auto GetViewport() const noexcept { return m_pViewport; }
        // get text anti-mode 
        auto GetTextAntimode() const noexcept { return static_cast<D2D1_TEXT_ANTIALIAS_MODE>(m_textAntiMode); }
        // get text anti-mode 
        void SetTextAntimode(D2D1_TEXT_ANTIALIAS_MODE mode) noexcept { m_textAntiMode = static_cast<decltype(m_textAntiMode)>(mode); }
        // is mouse captured control?
        auto IsCapturedControl(UIControl* c) noexcept { return m_pCapturedControl == c; };
        // is rendered
        auto IsRendered() const noexcept { return m_baBoolWindow.Test(XUIBaseWindow::Index_Rendered); }
        // copystring for control in this winddow
        auto CopyString(const char* str) noexcept { return m_oStringAllocator.CopyString(str); }
        // copystring for control in this winddow in safe way
        auto CopyStringSafe(const char* str) noexcept { auto s = this->CopyString(str); return s ? s : ""; }
        // render window in next frame
        void InvalidateWindow() noexcept { this->set_full_render_this_frame(); }
#ifdef _DEBUG
        // clear render info in debug mode
        void ClearRenderInfo() noexcept { this->clear_full_render_this_frame(); m_uUnitLength = 0; std::memset(m_apUnit, 0, sizeof(m_apUnit)); }
#else
        // clear render info
        void ClearRenderInfo() noexcept { this->clear_full_render_this_frame(); m_uUnitLength = 0; }
#endif
    public:
        // initialize viewport
        void InitializeViewport(UIViewport* viewport) noexcept;
        // do event
        bool DoEvent(const EventArgument& arg) noexcept;
        // render control in next frame
        void Invalidate(UIControl* ctrl) noexcept;
        // set the caret
        void SetCaretPos(UIControl* ctrl, float x, float y) noexcept;
        // create the caret
        void CreateCaret(UIControl* ctrl, float width, float height) noexcept;
        // show the caret
        void ShowCaret() noexcept;
        // hide the caret
        void HideCaret() noexcept;
        // set focus control
        void SetFocus(UIControl* ctrl) noexcept;
        // set hover track control
        void SetHoverTrack(UIControl* ctrl) noexcept;
        // find control
        auto FindControl(const char* name) noexcept ->UIControl*;
        // add control with name
        void AddNamedControl(UIControl* ctrl) noexcept;
        // set mouse capture
        void SetCapture(UIControl* control) noexcept;
        // release mouse capture
        void ReleaseCapture() noexcept;
    protected:
        // is SkipRender
        bool is_skip_render() const noexcept { return m_baBoolWindow.Test(Index_SkipRender); }
        // is CloseOnFocusKilled
        bool is_close_on_focus_killed() const noexcept { return m_baBoolWindow.Test(Index_CloseOnFocusKilled); }
        // is FullRenderingThisFrame
        bool is_full_render_this_frame() const noexcept { return m_baBoolWindow.Test(Index_FullRenderThisFrame); }
        // is NewSize
        bool is_new_size() const noexcept { return m_baBoolWindow.Test(Index_NewSize); }
    protected:
        // set CloseOnFocusKilled to true
        void set_close_on_focus_killed() noexcept { m_baBoolWindow.SetTrue(Index_CloseOnFocusKilled); }
        // set ExitOnClose to true
        void set_exit_on_close() noexcept { m_baBoolWindow.SetTrue(Index_ExitOnClose); }
        // set SkipRender to true
        void set_skip_render() noexcept { m_baBoolWindow.SetTrue(Index_SkipRender); }
        // clear SkipRender
        void clear_skip_render() noexcept { m_baBoolWindow.SetFalse(Index_SkipRender); }
        // set FullRenderingThisFrame to true
        void set_full_render_this_frame() noexcept { m_baBoolWindow.SetTrue(Index_FullRenderThisFrame); }
        // clear FullRenderingThisFrame
        void clear_full_render_this_frame() noexcept { m_baBoolWindow.SetFalse(Index_FullRenderThisFrame);  }
        // set NewSize to true
        void set_new_size() noexcept { m_baBoolWindow.SetTrue(Index_NewSize); }
        // clear FullRenderingThisFrame
        void clear_new_size() noexcept { m_baBoolWindow.SetFalse(Index_NewSize);  }
    protected:
        // resized, called from child-class
        void resized() noexcept;
    protected:
        // longui viewport
        UIViewport*             m_pViewport = nullptr;
        // parent window
        XUIBaseWindow*          m_pParent = nullptr;
        // children
        WindowVector            m_vChildren;
        // window handle
        HWND                    m_hwnd = nullptr;
        // TODO: mini size
        POINT                   m_miniSize;
        // now hover track control(only one)
        UIControl*              m_pHoverTracked = nullptr;
        // now focused control (only one)
        UIControl*              m_pFocusedControl = nullptr;
        // now dragdrop control (only one)
        UIControl*              m_pDragDropControl = nullptr;
        // now captured control (only one)
        UIControl*              m_pCapturedControl = nullptr;
        // window rect
        RectLTWH_L              m_rcWindow;
        // string allocator
        StringAllocator         m_oStringAllocator;
        // will use BitArray instead of them
        Helper::BitArray16      m_baBoolWindow;
        // mode for text anti-alias
        uint16_t                m_textAntiMode = D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
        // data length of m_apUnits
        uint32_t                m_uUnitLength = 0;
        // data for unit
        UIControl*              m_apUnit[LongUIDirtyControlSize];
        // dirty rects
        //RECT                    m_dirtyRects[LongUIDirtyControlSize];
        // current STGMEDIUM: begin with DWORD
        STGMEDIUM               m_curMedium;
        // control name ->map-> control pointer
        StringTable             m_hashStr2Ctrl;
    public:
        // debug info
#ifdef _DEBUG
        bool                    test_D2DERR_RECREATE_TARGET = false;
        bool                    debug_show = false;
        bool                    debug_unused[6];
        uint32_t                full_render_counter = 0;
        uint32_t                dirty_render_counter = 0;
#endif
        // last mouse point
        D2D1_POINT_2F           last_point = D2D1::Point2F(-1.f, -1.f);
        // clear color
        D2D1::ColorF            clear_color = D2D1::ColorF(D2D1::ColorF::White);
    };
    // system window
    class XUISystemWindow : public XUIBaseWindow {
        // super class
        using Super = XUIBaseWindow;
    public:
        // ctor
        XUISystemWindow(const Config::Window& config) noexcept;
        // dtor
        ~XUISystemWindow() noexcept;
    public:
        // move window
        virtual void MoveWindow(int32_t x, int32_t y) noexcept override;
        // resize window
        virtual void Resize(uint32_t w, uint32_t h) noexcept override;
    public:

    protected:
        // message id for TaskbarBtnCreated
        static const UINT s_uTaskbarBtnCreatedMsg;
    };
    // create builtin window
    auto CreateBuiltinWindow(const Config::Window& config) noexcept ->XUIBaseWindow*;
}