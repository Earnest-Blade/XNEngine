FROM mcr.microsoft.com/dotnet/runtime:7.0 AS base
WORKDIR /app

FROM mcr.microsoft.com/dotnet/sdk:7.0 AS build
WORKDIR /src
COPY ["xne.model.csproj", "./"]
RUN dotnet restore "xne.model.csproj"
COPY . .
WORKDIR "/src/"
RUN dotnet build "xne.model.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "xne.model.csproj" -c Release -o /app/publish /p:UseAppHost=false

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "xne.model.dll"]
