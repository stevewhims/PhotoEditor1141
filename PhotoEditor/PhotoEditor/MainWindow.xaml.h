#pragma once

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include "MainWindow.g.h"

#pragma pop_macro("GetCurrentTime")

namespace winrt::PhotoEditor::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        // Retreives collection of Photo objects for thumbnail view.
        Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> Photos() const
        {
            return m_photos;
        }

        // Event handlers for loading and rendering images.
        Windows::Foundation::IAsyncAction OnActivated(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::WindowActivatedEventArgs const&);
        Windows::Foundation::IAsyncAction OnContainerContentChanging(Microsoft::UI::Xaml::Controls::ListViewBase const&, Microsoft::UI::Xaml::Controls::ContainerContentChangingEventArgs const&);

    private:
        Windows::Foundation::IAsyncOperation<PhotoEditor::Photo> LoadImageInfoAsync(Windows::Storage::StorageFile const&);

        // Backing field for Photo collection.
        Windows::Foundation::Collections::IVector<IInspectable> m_photos{ nullptr };
    };
}

namespace winrt::PhotoEditor::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
